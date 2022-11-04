#include "unit.h"

#include <iostream>
#include <regex>

#pragma warning(push, 0)
#include <llvm/IR/DiagnosticPrinter.h>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/ADT/Optional.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"


#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>


#pragma warning(pop)

#include <sproc/src/fork.h>

#include <wall_e/src/lex.h>

#include "capabilities/constants_capability.h"
#include "../entities/type.h"
#include "../entities/value.h"
#include "../entities/function.h"
#include "../entities/argument.h"
#include "../entities/block.h"

#include "private.unit.h"

km2::llvm_backend::unit::unit(backend::backend *b) : backend::unit(b) {
    m_p = std::make_unique<unit_private>();

    m_p->context = std::make_unique<llvm::LLVMContext>();
    m_p->builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*m_p->context));
    m_p->module = std::make_unique<llvm::Module>("Module", *m_p->context);
}

//llvm::Value *km2::llvm_backend::unit::execute(const std::function<llvm::Value *(llvm::LLVMContext *, llvm::IRBuilder<> *, llvm::Module *)> &func) {
//    return func(m_p->context.get(), m_p->builder.get(), m_p->module.get());
//}

km2::backend::unit::ArgSettingStatus km2::llvm_backend::unit::set_arg(const std::string &name, backend::value *value) {
    if(m_p->stack.size() > 0) {
        auto& args = m_p->stack.top().args;
        const auto it = args.find(name);
        if (it == args.end()) {
            args[name] = value;
            return ArgSettingSuccess;
        }
        return ArgSettingDublicates;
    }
    return ArgSettingEmptyStack;
}

km2::backend::value *km2::llvm_backend::unit::arg(const std::string &name) const {
    if(m_p->stack.size() > 0) {
        const auto& args = m_p->stack.top().args;
        const auto it = args.find(name);
        if (it != args.end()) {
            return it->second;
        }
    }
    return nullptr;
}


km2::backend::block *km2::llvm_backend::unit::begin_block(
        const std::string &name,
        backend::function *func,
        const wall_e::str_vec &arg_names
        ) {
    auto entry_block = m_p->born_block(llvm::BasicBlock::Create(*m_p->context.get(), name, func->as<llvm_backend::function>()->data()));

    std::map<std::string, backend::value*> args;
    if(func) {
        int i = 0;
        for(const auto& arg : func->args()) {
            std::cout << "func: " << func->name() << ", arg: " << arg_names[i] << "\n";
            args[arg_names[i]] = arg;
            ++i;
        }
    }

    m_p->stack.push(unit_private::ctx { entry_block, args });
    return entry_block;
}

void km2::llvm_backend::unit::end_block() {
    if(m_p->stack.size() > 0) {
        m_p->stack.pop();
    }
}

void km2::llvm_backend::unit::setup_insert_point() {
    if(m_p->stack.size() > 0) {
        m_p->builder->SetInsertPoint(m_p->stack.top().block->data());
    }
}

km2::backend::function *km2::llvm_backend::unit::begin_entry(const std::string &name) {
    const auto result = m_p->born_function(llvm::Function::Create(
                llvm::FunctionType::get(llvm::Type::getInt32Ty(*m_p->context.get()), {}, false),
                llvm::Function::ExternalLinkage,
                name,
                m_p->module.get()
                ));
    begin_block(name + "_block", result);
    return result;
}

int km2::llvm_backend::unit::gen() {
    auto some_val = cap<constants_capability>()->uint32(42)->as<value>();

    std::string debug;
    llvm::raw_string_ostream s(debug);
    llvm::DiagnosticPrinterRawOStream p(s);

    p << *some_val->data();

    std::cout << "DEBUG: " << debug << std::endl;


    llvm::Function *parentFunction = m_p->builder->GetInsertBlock()
            ->getParent();
    // create temp builder to point to start of function
    llvm::IRBuilder<> TmpBuilder(&(parentFunction->getEntryBlock()), parentFunction->getEntryBlock().begin());
    // .begin() inserts this alloca at beginning of block


    llvm::AllocaInst *var = TmpBuilder.CreateAlloca(some_val->data()->getType());
    // resume our current position by using orig. builder
    m_p->builder->CreateStore(some_val->data(), var);
    return 0;
}

km2::backend::function *km2::llvm_backend::unit::create_sum_function() {
    /* Builds the following function:

        int sum(int a, int b) {
            int sum1 = 1 + 1;
            int sum2 = sum1 + a;
            int result = sum2 + b;
            return result;
        }
        */

    llvm::LLVMContext &context = m_p->module->getContext();

    // Define function's signature
    wall_e::vec<llvm::Type*> integers(2, m_p->builder->getInt32Ty());
    auto func_type = llvm::FunctionType::get(m_p->builder->getInt32Ty(), integers, false);

    // create the function "sum" and bind it to the module with ExternalLinkage,
    // so we can retrieve it later
    auto foo_func = llvm::Function::Create(
                func_type, llvm::Function::ExternalLinkage, "sum", m_p->module.get()
                );

    // Define the entry block and fill it with an appropriate code
    auto *entry = llvm::BasicBlock::Create(context, "entry", foo_func);
    m_p->builder->SetInsertPoint(entry);

    // Add constant to itself, to visualize constant folding
    llvm::Value *constant = llvm::ConstantInt::get(m_p->builder->getInt32Ty(), 0x1);
    auto *sum1 = m_p->builder->CreateAdd(constant, constant, "sum1");

    // Retrieve arguments and proceed with further adding...
    auto args = foo_func->arg_begin();
    llvm::Value *arg1 = &(*args);
    args = std::next(args);
    llvm::Value *arg2 = &(*args);
    auto *sum2 = m_p->builder->CreateAdd(sum1, arg1, "sum2");
    auto *result = m_p->builder->CreateAdd(sum2, arg2, "result");

    // ...and return
    m_p->builder->CreateRet(result);

    // Verify at the end
    llvm::verifyFunction(*foo_func);
    return m_p->born_function(foo_func);
}

int km2::llvm_backend::unit::aaa() {
    auto sum_func = create_sum_function();

    const auto m_entryPoint = llvm::Function::Create(
                llvm::FunctionType::get(llvm::Type::getInt32Ty(*m_p->context.get()), {}, false),
                llvm::Function::ExternalLinkage,
                "km2_main",
                m_p->module.get()
                );

    llvm::BasicBlock *entryBasicBlock = llvm::BasicBlock::Create(*m_p->context.get(), "entry", m_entryPoint);
    m_p->builder->SetInsertPoint(entryBasicBlock);

    auto a = m_p->builder->CreateCall(sum_func->as<function>()->data(), {
                                       cap<constants_capability>()->uint32(10)->as<value>()->data(),
                                       cap<constants_capability>()->uint32(20)->as<value>()->data()
                                   });

    auto printf_func = m_p->module->getOrInsertFunction(
                "printf",
                llvm::FunctionType::get(
                    llvm::IntegerType::getInt32Ty(*m_p->context.get()),
                    llvm::PointerType::get(llvm::Type::getInt8Ty(*m_p->context.get()), 0),
                    true
                    )
                );

    const auto format_str_ptr = cap<constants_capability>()->string_cstr(".fmt", "Hello World: %d\n");

    m_p->builder->CreateCall(printf_func, {
                              format_str_ptr->as<value>()->data(),
                              a
                          });





    m_p->builder->CreateRet(a);

    print();

    return 0;
}

km2::backend::function *km2::llvm_backend::unit::proto(const std::string& name, wall_e::vec<backend::type*> arg_types, backend::type* result_type, bool is_var_arg) {
    return m_p->born_function(llvm::Function::Create(
                llvm::FunctionType::get(
                    result_type->as<type>()->data(),
                    m_p->types_vec_data(arg_types),
                    is_var_arg
                    ),
                llvm::Function::ExternalLinkage,
                name,
                m_p->module.get()
                ));
}

void *km2::llvm_backend::unit::inline_asm(const std::string& text) {
/**
 *  TODO
    llvm::InlineAsm *IA = llvm::InlineAsm::get(nullptr, text, "", true, false);

    llvm::CallInst *result = m_builder->CreateCall(IA, {});

    //result->addAttribute(llvm::AttributeSet::FunctionIndex, llvm::Attribute::NoUnwind);
    return result;
    */
    return nullptr;
}

void km2::llvm_backend::unit::print() {
    llvm::outs() << llvm::raw_ostream::CYAN << "\nMODULE:\n" << llvm::raw_ostream::YELLOW << *m_p->module.get() << llvm::raw_ostream::RESET;
}

void km2::llvm_backend::unit::print_functions() {
    llvm::outs() << "functions:\n";
    for(const auto &f : m_p->module->functions()) {
        llvm::outs() << "  " << "func: " << f.getName() << "\n";
        for(const auto& b : f.getBasicBlockList()) {
            llvm::outs() << "    " << b.getName() << "\n";
        }
    }
    llvm::outs().flush();
}

std::string km2::llvm_backend::unit::llvm_assembly() const {
    std::string result;
    llvm::raw_string_ostream stream(result);
    stream << *m_p->module.get();
    return result;
}

wall_e::either<std::string, int> km2::llvm_backend::unit::run_jit(backend::function *entry_point) {
    if(entry_point) {
        llvm::outs().flush();
        llvm::InitializeNativeTarget();
        LLVMInitializeNativeAsmPrinter();
        LLVMInitializeNativeAsmParser();

        auto engine = llvm::EngineBuilder(std::move(m_p->module)).create();
        std::vector<llvm::GenericValue> noargs;
        std::cout << "JIT BEGIN" << std::endl;
        auto result = engine->runFunction(entry_point->as<function>()->data(), noargs);
        int int_res = result.IntVal.getSExtValue();
        std::cout << "JIT END: " << int_res << std::endl;
        delete engine;
        return wall_e::right(int_res);
    } else {
        std::cout << "JIT ERR: entry point not set" << std::endl;
    }
    return wall_e::left<std::string>("entry point not specified");
}

wall_e::either<std::string, int> km2::llvm_backend::unit::compile(const std::string &output_path) {
    auto target_triple = llvm::sys::getDefaultTargetTriple();
    llvm::outs() << "TargetTriple: " << target_triple << "\n";

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(target_triple, error);

    if (!target) {
        return wall_e::left(error);
    }

    llvm::outs() << "target: " << target << "\n";

    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    auto reloc_model = llvm::Optional<llvm::Reloc::Model>();
    auto target_machine = target->createTargetMachine(target_triple, cpu, features, opt, reloc_model);
    llvm::outs() << "reloc_model: " << reloc_model << "\n";
    llvm::outs() << "target_machine: " << target_machine << "\n";


    m_p->module->setDataLayout(target_machine->createDataLayout());
    m_p->module->setTargetTriple(target_triple);


    std::error_code EC;
    llvm::raw_fd_ostream dest(output_path, EC, llvm::sys::fs::OF_None);

    if (EC) {
        return wall_e::left("Could not open file: " + EC.message());
    }

    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;

    if (target_machine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        return wall_e::left(std::string("TargetMachine can't emit a file of this type"));
    }

    pass.run(*m_p->module);
    dest.flush();

    return wall_e::right(0);
}

wall_e::either<std::string, int> km2::llvm_backend::unit::make_executable(const std::string &output_path) {
    const auto object_path = output_path + ".o";
    if(const auto err = compile(object_path).left()) {
        return err;
    }

    const auto link_result = sproc::system("clang++ " + object_path + " -o " + output_path);
    if(link_result.ext_code != 0) {
        return wall_e::left(link_result.err);
    }

    return wall_e::right(0);
}


km2::llvm_backend::block* km2::llvm_backend::unit_private::born_block(llvm::BasicBlock* data) {
    blocks.push_back(block::make_uniq(data));
    return blocks.back().get();
}
km2::llvm_backend::function* km2::llvm_backend::unit_private::born_function(llvm::Function* data) {
    functions.push_back(function::make_uniq(data));
    return functions.back().get();
}

inline llvm::ArrayRef<llvm::Type *> km2::llvm_backend::unit_private::types_vec_data(const wall_e::vec<backend::type*> &types) {
    return llvm::ArrayRef<llvm::Type*>(types.map<llvm::Type*>([](backend::type* t){ return t->as<type>()->data(); }));
}
