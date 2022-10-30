#include "translation_unit.h"

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
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
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

#include <src/km2/translation_unit/capabilities/constants_capability.h>

km2::translation_unit::translation_unit() {
    m_context = std::make_unique<llvm::LLVMContext>();
    m_builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*m_context));
    m_module = std::make_unique<llvm::Module>("Module", *m_context);

}

llvm::Value *km2::translation_unit::execute(const std::function<llvm::Value *(llvm::LLVMContext *, llvm::IRBuilder<> *, llvm::Module *)> &func) {
    return func(m_context.get(), m_builder.get(), m_module.get());
}


km2::translation_unit::ArgSettingStatus km2::translation_unit::set_arg(const std::string &name, llvm::Value *value) {
    if(m_stack.size() > 0) {
        auto& args = m_stack.top().args;
        const auto it = args.find(name);
        if (it == args.end()) {
            args[name] = value;
            return ArgSettingSuccess;
        }
        return ArgSettingDublicates;
    }
    return ArgSettingEmptyStack;
}

llvm::Value *km2::translation_unit::arg(const std::string &name) const {
    if(m_stack.size() > 0) {
        const auto& args = m_stack.top().args;
        const auto it = args.find(name);
        if (it != args.end()) {
            return it->second;
        }
    }
    return nullptr;
}

llvm::FunctionType *km2::translation_unit::func(llvm::Type* returnType, llvm::ArrayRef<llvm::Type*> argTypes) {
    return llvm::FunctionType::get(returnType, argTypes, false);
}

llvm::BasicBlock *km2::translation_unit::begin_block(
        const std::string &name,
        llvm::Function *func,
        const std::vector<std::string> &argNames
        ) {
    llvm::BasicBlock *entryBasicBlock = llvm::BasicBlock::Create(*m_context.get(), name, func);

    std::map<std::string, llvm::Value*> args;
    if(func) {
        int i = 0;
        for(auto it = func->arg_begin(); it != func->arg_end() && i < argNames.size(); ++it) {
            std::cout << "func: " << func->getName().str() << ", arg: " << argNames[i] << "\n";
            args[argNames[i]] = &(*it);
            ++i;
        }
    }

    m_stack.push(ctx { entryBasicBlock, args });
    return entryBasicBlock;
}

void km2::translation_unit::end_block() {
    if(m_stack.size() > 0) {
        m_stack.pop();
    }
}

void km2::translation_unit::setup_insert_point() {
    if(m_stack.size() > 0) {
        m_builder->SetInsertPoint(m_stack.top().block);
    }
}

llvm::Function *km2::translation_unit::begin_entry(const std::string &name) {
    const auto result = llvm::Function::Create(
                llvm::FunctionType::get(llvm::Type::getInt32Ty(*m_context.get()), {}, false),
                llvm::Function::ExternalLinkage,
                name,
                m_module.get()
                );
    begin_block(name + "_block", result);
    return result;
}

int km2::translation_unit::gen() {
    auto someVal = cap<constants_capability>()->uint32(42);

    std::string debug;
    llvm::raw_string_ostream s(debug);
    llvm::DiagnosticPrinterRawOStream p(s);

    p << *someVal;

    std::cout << "DEBUG: " << debug << std::endl;


    llvm::Function *parentFunction = m_builder->GetInsertBlock()
            ->getParent();
    // create temp builder to point to start of function
    llvm::IRBuilder<> TmpBuilder(&(parentFunction->getEntryBlock()), parentFunction->getEntryBlock().begin());
    // .begin() inserts this alloca at beginning of block


    llvm::AllocaInst *var = TmpBuilder.CreateAlloca(someVal->getType());
    // resume our current position by using orig. builder
    m_builder->CreateStore(someVal, var);
    return 0;
}

llvm::Function *km2::translation_unit::createSumFunction() {
    /* Builds the following function:

        int sum(int a, int b) {
            int sum1 = 1 + 1;
            int sum2 = sum1 + a;
            int result = sum2 + b;
            return result;
        }
        */

    llvm::LLVMContext &context = m_module->getContext();

    // Define function's signature
    std::vector<llvm::Type*> Integers(2, m_builder->getInt32Ty());
    auto *funcType = llvm::FunctionType::get(m_builder->getInt32Ty(), Integers, false);

    // create the function "sum" and bind it to the module with ExternalLinkage,
    // so we can retrieve it later
    auto *fooFunc = llvm::Function::Create(
                funcType, llvm::Function::ExternalLinkage, "sum", m_module.get()
                );

    // Define the entry block and fill it with an appropriate code
    auto *entry = llvm::BasicBlock::Create(context, "entry", fooFunc);
    m_builder->SetInsertPoint(entry);

    // Add constant to itself, to visualize constant folding
    llvm::Value *constant = llvm::ConstantInt::get(m_builder->getInt32Ty(), 0x1);
    auto *sum1 = m_builder->CreateAdd(constant, constant, "sum1");

    // Retrieve arguments and proceed with further adding...
    auto args = fooFunc->arg_begin();
    llvm::Value *arg1 = &(*args);
    args = std::next(args);
    llvm::Value *arg2 = &(*args);
    auto *sum2 = m_builder->CreateAdd(sum1, arg1, "sum2");
    auto *result = m_builder->CreateAdd(sum2, arg2, "result");

    // ...and return
    m_builder->CreateRet(result);

    // Verify at the end
    llvm::verifyFunction(*fooFunc);
    return fooFunc;
}

int km2::translation_unit::aaa() {
    auto sumFunc = createSumFunction();

    const auto m_entryPoint = llvm::Function::Create(
                llvm::FunctionType::get(llvm::Type::getInt32Ty(*m_context.get()), {}, false),
                llvm::Function::ExternalLinkage,
                "km2_main",
                m_module.get()
                );

    llvm::BasicBlock *entryBasicBlock = llvm::BasicBlock::Create(*m_context.get(), "entry", m_entryPoint);
    m_builder->SetInsertPoint(entryBasicBlock);

    auto a = m_builder->CreateCall(sumFunc, {
                                       cap<constants_capability>()->uint32(10),
                                       cap<constants_capability>()->uint32(20)
                                   });

    auto printf_func = m_module->getOrInsertFunction(
                "printf",
                llvm::FunctionType::get(
                    llvm::IntegerType::getInt32Ty(*m_context.get()),
                    llvm::PointerType::get(llvm::Type::getInt8Ty(*m_context.get()), 0),
                    true
                    )
                );

    const auto format_str_ptr = cap<constants_capability>()->string_cstr(".fmt", "Hello World: %d\n");

    m_builder->CreateCall(printf_func, {
                              format_str_ptr,
                              a
                          });





    m_builder->CreateRet(a);

    print();

    return 0;
}

llvm::Function *km2::translation_unit::proto(const std::string& name, std::vector<llvm::Type*> arg_types, llvm::Type* result_type, bool is_var_arg) {
    return llvm::Function::Create(
                llvm::FunctionType::get(
                    result_type,
                    arg_types,
                    is_var_arg
                    ),
                llvm::Function::ExternalLinkage,
                name,
                m_module.get()
                );
}

llvm::CallInst *km2::translation_unit::inline_asm(const std::string& text) {

    llvm::InlineAsm *IA = llvm::InlineAsm::get(nullptr, text, "", true, false);

    llvm::CallInst *result = m_builder->CreateCall(IA, {});

    //result->addAttribute(llvm::AttributeSet::FunctionIndex, llvm::Attribute::NoUnwind);
    return result;
}

void km2::translation_unit::print() {
    llvm::outs() << llvm::raw_ostream::CYAN << "\nMODULE:\n" << llvm::raw_ostream::YELLOW << *m_module.get() << llvm::raw_ostream::RESET;
}

void km2::translation_unit::print_functions() {
    llvm::outs() << "functions:\n";
    for(const auto &f : m_module->functions()) {
        llvm::outs() << "  " << "func: " << f.getName() << "\n";
        for(const auto& b : f.getBasicBlockList()) {
            llvm::outs() << "    " << b.getName() << "\n";
        }
    }
    llvm::outs().flush();
}

std::string km2::translation_unit::llvm_assembly() const {
    std::string result;
    llvm::raw_string_ostream stream(result);
    stream << *m_module.get();
    return result;
}

wall_e::either<std::string, int> km2::translation_unit::run_jit(llvm::Function *entry_point) {
    if(entry_point) {
        llvm::outs().flush();
        llvm::InitializeNativeTarget();
        LLVMInitializeNativeAsmPrinter();
        LLVMInitializeNativeAsmParser();

        auto engine = llvm::EngineBuilder(std::move(m_module)).create();
        std::vector<llvm::GenericValue> noargs;
        std::cout << "JIT BEGIN" << std::endl;
        auto result = engine->runFunction(entry_point, noargs);
        int int_res = result.IntVal.getSExtValue();
        std::cout << "JIT END: " << int_res << std::endl;
        delete engine;
        return wall_e::right(int_res);
    } else {
        std::cout << "JIT ERR: entry point not set" << std::endl;
    }
    return wall_e::left<std::string>("entry point not specified");
}

wall_e::either<std::string, int> km2::translation_unit::compile(const std::string &output_path) {
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


    m_module->setDataLayout(target_machine->createDataLayout());
    m_module->setTargetTriple(target_triple);


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

    pass.run(*m_module);
    dest.flush();

    return wall_e::right(0);
}

wall_e::either<std::string, int> km2::translation_unit::make_executable(const std::string &output_path) {
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


llvm::LLVMContext* km2::translation_unit::llvm_context() const {
    return m_context.get();
}

llvm::Module* km2::translation_unit::llvm_module() const {
    return m_module.get();
}

llvm::IRBuilder<>* km2::translation_unit::llvm_builder() const {
    return m_builder.get();
}

km2::translation_unit::~translation_unit() {
    for(const auto& c : m_capabilities) {
        delete c.second;
    }
}
