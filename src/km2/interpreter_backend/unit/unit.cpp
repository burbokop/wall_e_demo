#include "unit.h"

#include <iostream>
#include <regex>

#include <sproc/src/fork.h>
#include <src/km2/interpreter_backend/instructions/call_instruction.h>
#include <wall_e/src/lex.h>

#include "capabilities/constants_capability.h"
#include "capabilities/type_capability.h"
#include "../entities/type.h"
#include "../entities/value.h"
#include "../entities/constant.h"
#include "../entities/function.h"
#include "../entities/argument.h"
#include "../entities/block.h"
#include "../instructions/execution_ctx.h"
#include <math.h>
#include "../instructions/call_instruction.h"

namespace km2 {
namespace interpreter_backend {

struct __plug_native_function_provider : public native_function_provider {
    // native_function_provider interface
public:
    virtual dyn_function::native_func native_func(const std::string &name) const override;
};

dyn_function::native_func __plug_native_function_provider::native_func(const std::string &name) const {
    static wall_e::map<std::string, dyn_function::native_func> fs = {
        {
            "printf",
            [](execution_ctx* ctx, const wall_e::vec<dyn_value::ptr>& args) -> dyn_value::ptr {
                assert(args.size() > 0);
                auto fmt = args[0]->value(ctx).value<std::string>();

                bool prev_precent = false;
                std::size_t i = 1;
                auto it = fmt.begin();
                while(it != fmt.end()) {
                    if(*it == '%') {
                        it = fmt.erase(it, it + 2);
                        auto val = args[i++]->value(ctx).to_string();
                        it = fmt.insert(it, val.begin(), val.end());
                    } else {
                        ++it;
                    }
                }
                const auto result = printf(fmt.c_str());
                fflush(stdout);
                return dyn_value::from_variant(result);
            }
        }, {
            "pow",
            [](execution_ctx* ctx, const wall_e::vec<dyn_value::ptr>& args) -> dyn_value::ptr {
                assert(args.size() == 2);
                return dyn_value::from_variant(std::pow(
                    args[0]->value(ctx).value<double>(),
                    args[1]->value(ctx).value<double>()
                ));
            }
        }
    };
    return fs.find_opt(name).value_or(nullptr);
}

static const __plug_native_function_provider* plug_native_function_provider = new __plug_native_function_provider();
}
}


km2::interpreter_backend::unit::unit(const backend::backend *b) : backend::unit(b) {
    //m_p = std::make_unique<unit_private>();

    m_ctx = std::make_shared<execution_ctx>();
    //m_p->builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*m_p->context));
    //m_p->module = std::make_unique<llvm::Module>("Module", *m_p->context);
}

//llvm::Value *km2::llvm_backend::unit::execute(const std::function<llvm::Value *(llvm::LLVMContext *, llvm::IRBuilder<> *, llvm::Module *)> &func) {
//    return func(m_p->context.get(), m_p->builder.get(), m_p->module.get());
//}

km2::backend::unit::ArgSettingStatus km2::interpreter_backend::unit::set_arg(const std::string &name, backend::value *value) {
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

km2::backend::value *km2::interpreter_backend::unit::arg(const std::string &name) const {
    if(m_stack.size() > 0) {
        const auto& args = m_stack.top().args;
        const auto it = args.find(name);
        if (it != args.end()) {
            return it->second;
        }
    }
    return nullptr;
}
/*
begin_block in km2_main_block : { name: km2_main_block, instructions: [] }
    begin_block in fn2_block : { name: fn2_block, instructions: [] }
        create_void_return in fn2_block
    end_block: { name: fn2_block, instructions: [ 0x1a78f0c76a0 ] }
    begin_block in print_block : { name: print_block, instructions: [] }
        create_call in print_block: pow [ km2::interpreter_backend::argument { type: 0x1a78f0c7020, dyn: stack_ptr(0) }, km2::interpreter_backend::constant { type: 0x1a78f039d20, dyn: 0x1a78f0397e0 } ]
        create_call in print_block: printf [ km2::interpreter_backend::constant { type: 0x1a78f039dc0, dyn: 0x1a78f0396b0 }, km2::interpreter_backend::constant { type: 0x1a78f0c7600, dyn: 0x1a78f0bf040 } ]
        create_void_return in print_block
    end_block: { name: print_block, instructions: [ 0x1a78f08b9b0, 0x1a78f08b370, 0x1a78f03a000 ] }

    create_call in print_block: print [ km2::interpreter_backend::constant { type: 0x1a78f039fa0, dyn: 0x1a78f04e030 } ]
    create_return in print_block
end_block: { name: km2_main_block, instructions: [] }
*/
km2::backend::block *km2::interpreter_backend::unit::begin_block(
        const std::string &name,
        backend::function *func,
        const wall_e::str_vec &arg_names
        ) {

    auto entry_block = born_block(m_ctx.get(), name, func->as<function>());
    std::cout << "begin_block in " << name << " : " << entry_block << std::endl;

    std::map<std::string, backend::value*> args_map;
    if(func) {
        int i = 0;
        const auto& args = func->args();
        if(args.size() != arg_names.size()) {
            std::stringstream err_ss;
            err_ss << "arg names count (" << args << ") not match to arg names count (" << arg_names << ")";
            throw std::runtime_error(err_ss.str());
        }
        for(const auto& t : args) {
            std::cout << "func: " << func->name() << ", arg: " << arg_names[i] << "\n";
            args_map[arg_names[i]] = t;
            ++i;
        }
    }

    if(entry_block) {
        m_stack.push(ctx { .block = entry_block, .args = args_map, .offset = args_map.size() });
        setup_insert_point();
        return entry_block;
    }
    return nullptr;
}

void km2::interpreter_backend::unit::end_block() {
    if(m_stack.size() > 0) {
        std::cout << "end_block: " << m_stack.top().block << std::endl;
        m_stack.pop();
    } else {
        std::cout << "end_block: warning: empty stack" << std::endl;
    }
    setup_insert_point();
}

void km2::interpreter_backend::unit::setup_insert_point() {
    if(m_stack.size() > 0) {
        m_insert_point = m_stack.top().block;
    }
}

km2::backend::function *km2::interpreter_backend::unit::begin_entry(const std::string &name) {
    const auto result = born_function(
                name,
                cap<km2::backend::type_capability>()->as<type_capability>()->int32()->as<type>(),
                {},
                false,
                plug_native_function_provider
                );
    begin_block(name + "_block", result, {});
    return result;
}

int km2::interpreter_backend::unit::gen() {
    /*
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
    */
    return 0;
}

km2::backend::function *km2::interpreter_backend::unit::create_sum_function() {
    /* Builds the following function:

        int sum(int a, int b) {
            int sum1 = 1 + 1;
            int sum2 = sum1 + a;
            int result = sum2 + b;
            return result;
        }
        */

    /*
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
    */
    return nullptr;
}

int km2::interpreter_backend::unit::aaa() {
    /*
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
    */

    return 0;
}

km2::backend::function *km2::interpreter_backend::unit::proto(const std::string& name, wall_e::vec<backend::type*> arg_types, backend::type* result_type, bool is_var_arg) {
    return born_function(
                name,
                result_type->as<type>(),
                arg_types.map<type*>(type::downcast),
                is_var_arg,
                plug_native_function_provider
                );
}

void km2::interpreter_backend::unit::create_void_return() {
    if(m_insert_point) {
        std::cout << "create_void_return in " << m_insert_point->name() << std::endl;
    } else {
        std::cout << "create_void_return in null" << std::endl;
    }

    if(m_insert_point) {
        m_insert_point->append_instruction(new ret_instruction(m_ctx.get()));
    }
}

void km2::interpreter_backend::unit::create_return(km2::backend::value *val) {
    if(m_insert_point) {
        std::cout << "create_return in " << m_insert_point->name() << std::endl;
    } else {
        std::cout << "create_return in null" << std::endl;
    }

    if(m_insert_point) {
        m_insert_point->append_instruction(new ret_instruction(m_ctx.get()));
    }
}

km2::backend::value *km2::interpreter_backend::unit::create_call(backend::function *func, const wall_e::vec<backend::value*> &args) {
    if(m_insert_point) {
        std::cout << "create_call in " << m_insert_point->name() << ": " << func->name() << " " << args << std::endl;
    } else {
        std::cout << "create_call in null" << std::endl;
    }

    if(m_insert_point) {
        if(const auto& dyn = func->as<function>()->dyn()) {
            m_insert_point->append_instruction(new call_instruction(
                                                   m_ctx.get(),
                                                   dyn,
                                                   args
                                                        .map<value*>(value::downcast)
                                                        .map<dyn_value::ptr>(&value::dyn)
                                                   ));
            return born_constant(
                        func->return_type()->as<type>(),
                        !m_stack.empty()
                        ? dyn_value::from_stack_ptr(m_stack.top().offset++)
                        : dyn_value::ptr()
                          );
        } else {
            std::cerr << "err: dyn function not ready yet (func: " << func->name() << ")" << std::endl;
        }

    } else {
        std::cerr << "err: no insert point" << std::endl;
    }
    return nullptr;
}

void km2::interpreter_backend::unit::link() {
    m_ctx->clear_instructions();
    for(const auto& b : m_blocks) {
        m_ctx->append_instructions(b->instructions());
    }
    for(const auto& i : m_ctx->instructions()) {
        i->link();
    }
}

void *km2::interpreter_backend::unit::inline_asm(const std::string& text) {
/**
 *  TODO
    llvm::InlineAsm *IA = llvm::InlineAsm::get(nullptr, text, "", true, false);

    llvm::CallInst *result = m_builder->CreateCall(IA, {});

    //result->addAttribute(llvm::AttributeSet::FunctionIndex, llvm::Attribute::NoUnwind);
    return result;
    */
    return nullptr;
}

void km2::interpreter_backend::unit::print() {
    std::cout << "unit:" << std::endl;
    std::cout << "  functions:" << std::endl;
    for(auto f : m_functions) {
        if(const auto& dyn = f->as<function>()->dyn()) {
            std::cout << "    func: " << f->name() << (dyn->is_native() ? " [ native ]" : "") << std::endl;
        } else {
            std::cout << "    func: " << f->name() << " [ nulldyn ]" << std::endl;
        }
        for(const auto& b : f->blocks()) {
            std::cout << "      " << b->name() << std::endl;            const auto& instruction = b->as<block>()->instructions();
            for(std::size_t i = 0; i < instruction.size(); ++i) {
                instruction[i]->print(std::cout << "        " << "#" << i << ": ") << std::endl;
            }
        }
    }

    link();
    m_ctx->print(std::cout);


    //llvm::outs() << llvm::raw_ostream::CYAN << "\nMODULE:\n" << llvm::raw_ostream::YELLOW << *m_p->module.get() << llvm::raw_ostream::RESET;
}

void km2::interpreter_backend::unit::print_functions() {
    std::cout << "functions:" << std::endl;
    for(auto f : m_functions) {
        std::cout << "  " << "func: " << f->name() << std::endl;
        for(const auto& b : f->blocks()) {
            std::cout << "    " << b->name() << std::endl;
        }
    }
}

std::string km2::interpreter_backend::unit::llvm_assembly() const {
    std::string result;
    /*
    llvm::raw_string_ostream stream(result);
    stream << *m_p->module.get();
    */
    return result;
}

wall_e::either<std::string, int> km2::interpreter_backend::unit::run_jit(backend::function *entry_point) {
    if(entry_point) {


        link();
        //m_ctx->set_verbose(true);
        if(m_ctx->verbose()) {
            entry_point->print(std::cout << "running jit. entry: ") << std::endl;
        }
        m_ctx->exec(entry_point->as<function>()->dyn());

        //print();

        /*
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
        */
        return wall_e::right<int>(0);
    } else {
        std::cout << "JIT ERR: entry point not set" << std::endl;
    }
    return wall_e::left<std::string>("entry point not specified");
}

wall_e::either<std::string, int> km2::interpreter_backend::unit::compile(const std::string &output_path) {
    /*
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
    */
    return wall_e::left<std::string>("not implemented");
}

wall_e::either<std::string, int> km2::interpreter_backend::unit::make_executable(const std::string &output_path) {
    const auto object_path = output_path + ".o";
    if(const auto err = compile(object_path).left()) {
        return err;
    }

    return wall_e::right(0);
}


km2::interpreter_backend::block* km2::interpreter_backend::unit::born_block(
        execution_ctx* ctx,
        const std::string& name,
        function* f
        ) {
    return m_blocks.make(ctx, name, f);
}

km2::interpreter_backend::function* km2::interpreter_backend::unit::born_function(
        const std::string& name,
        type* return_type,
        const wall_e::vec<type*>& arg_types,
        bool is_var_arg,
        const native_function_provider* native_function_provider
        ) {
    return m_functions.make(name, return_type, arg_types, is_var_arg, native_function_provider);
}

km2::interpreter_backend::constant *km2::interpreter_backend::unit::born_constant(
        type *type,
        const dyn_value::ptr& val
        ) {
    return m_values.make(type, val);
}


