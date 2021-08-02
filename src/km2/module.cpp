#include "module.h"

#include <llvm/IR/DiagnosticPrinter.h>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRPrintingPasses.h"
#include <iostream>
#include <regex>

#include "llvm/Transforms/Scalar.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/IR/InlineAsm.h"


#include <wall_e/src/lex.h>

km2::module::module() {
    m_context = std::make_unique<llvm::LLVMContext>();
    m_builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*m_context));
    m_module = std::make_unique<llvm::Module>("Module", *m_context);

}

llvm::Value *km2::module::execute(const std::function<llvm::Value *(llvm::LLVMContext *, llvm::IRBuilder<> *, llvm::Module *)> &func) {
    return func(m_context.get(), m_builder.get(), m_module.get());
}

llvm::ConstantInt *km2::module::uint32(uint32_t value) {
    return llvm::ConstantInt::getSigned((llvm::Type::getInt32Ty(*m_context)), value);
}

llvm::ConstantInt *km2::module::uintptr(uintptr_t value) {
    return llvm::ConstantInt::getSigned(m_builder->getIntPtrTy(m_module->getDataLayout()), value);
}

llvm::ConstantInt *km2::module::uint(uint64_t value, size_t size) {
    return llvm::ConstantInt::getSigned(m_builder->getIntNTy(size), value);
}

llvm::Constant *km2::module::float64(double value) {
    return llvm::ConstantFP::get(llvm::Type::getDoubleTy(*m_context.get()), value);
}

km2::module::ArgSettingStatus km2::module::setArg(const std::string &name, llvm::Value *value) {
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

llvm::Value *km2::module::arg(const std::string &name) const {
    if(m_stack.size() > 0) {
        const auto& args = m_stack.top().args;
        const auto it = args.find(name);
        if (it != args.end()) {
            return it->second;
        }
    }
    return nullptr;
}

llvm::FunctionType *km2::module::func(llvm::Type* returnType, llvm::ArrayRef<llvm::Type*> argTypes) {
    return llvm::FunctionType::get(returnType, argTypes, false);
}

llvm::Value *km2::module::string_const_ptr(const std::string &name, const std::string &text) {
    const auto str = llvm::ConstantDataArray::getString(*m_context.get(), text, true);

    const auto global_str = new llvm::GlobalVariable(
                *m_module.get(),
                str->getType(),
                true,
                llvm::GlobalValue::ExternalLinkage,
                str,
                name
                );

    return m_builder->CreateGEP(global_str, uintptr(0));
}


llvm::BasicBlock *km2::module::beginBlock(
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

void km2::module::endBlock() {
    if(m_stack.size() > 0) {
        m_stack.pop();
    }
}

void km2::module::setupInsertPoint() {
    if(m_stack.size() > 0) {
        m_builder->SetInsertPoint(m_stack.top().block);
    }
}

llvm::Function *km2::module::beginEntry(const std::string &name) {
    const auto result = llvm::Function::Create(
                llvm::FunctionType::get(llvm::Type::getInt32Ty(*m_context.get()), {}, false),
                llvm::Function::ExternalLinkage,
                name,
                m_module.get()
                );
    beginBlock(name + "_block", result);
    return result;
}

int km2::module::gen() {
    auto someVal = uint32(42);

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

llvm::Function *km2::module::createSumFunction() {
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

int km2::module::aaa() {
    auto sumFunc = createSumFunction();


    const auto m_entryPoint = llvm::Function::Create(
                llvm::FunctionType::get(llvm::Type::getInt32Ty(*m_context.get()), {}, false),
                llvm::Function::ExternalLinkage,
                "main",
                m_module.get()
                );

    llvm::BasicBlock *entryBasicBlock = llvm::BasicBlock::Create(*m_context.get(), "entry", m_entryPoint);
    m_builder->SetInsertPoint(entryBasicBlock);

    auto a = m_builder->CreateCall(sumFunc, {
                                       uint32(10),
                                       uint32(20)
                                   });

    auto printf_func = m_module->getOrInsertFunction(
                "printf",
                llvm::FunctionType::get(
                    llvm::IntegerType::getInt32Ty(*m_context.get()),
                    llvm::PointerType::get(llvm::Type::getInt8Ty(*m_context.get()), 0),
                    true
                    )
                );

    const auto format_str_ptr = string_const_ptr(".fmt", "Hello World: %d\n");

    m_builder->CreateCall(printf_func, {
                              format_str_ptr,
                              a
                          });





    m_builder->CreateRet(a);

    print();

    return 0;
}

llvm::Function *km2::module::proto(llvm::Type* resultType, std::vector<llvm::Type*> argTypes, const std::string& name, bool isVarArg) {
    return llvm::Function::Create(
                llvm::FunctionType::get(
                    resultType,
                    argTypes,
                    isVarArg
                    ),
                llvm::Function::ExternalLinkage,
                name,
                m_module.get()
                );
}

llvm::CallInst *km2::module::inline_asm(const std::string& text) {

    llvm::InlineAsm *IA = llvm::InlineAsm::get(nullptr, text, "", true, false);

    llvm::CallInst *result = m_builder->CreateCall(IA, {});

    //result->addAttribute(llvm::AttributeSet::FunctionIndex, llvm::Attribute::NoUnwind);
    return result;
}

void km2::module::print() {
    llvm::outs() << llvm::raw_ostream::CYAN << "\nMODULE:\n" << llvm::raw_ostream::YELLOW << *m_module.get() << llvm::raw_ostream::RESET;
}

std::string km2::module::llvmAssembly() const {
    std::string result;
    llvm::raw_string_ostream stream(result);
    stream << *m_module.get();
    return result;
}

wall_e::either<std::string, int> km2::module::runJit(llvm::Function *entry_point) {
    //std::cout << "\nRUN JIT\n";
    //
    //llvm::outs() << "Blocks:\n";
    //for(const auto &f : m_module->functions()) {
    //    //llvm::outs() << "\t" << "func:" << "\n";
    //    for(const auto& b : f.getBasicBlockList()) {
    //        llvm::outs() << "\t" << b.getName() << "\n";
    //        //for (auto it = b.begin()) {
    //        //
    //        //}
    //    }
    //}

    llvm::TargetOptions Opts;
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    std::unique_ptr<llvm::RTDyldMemoryManager> MemMgr(new llvm::SectionMemoryManager());
    llvm::EngineBuilder factory(std::move(m_module));
    factory.setEngineKind(llvm::EngineKind::JIT);
    factory.setTargetOptions(Opts);
    factory.setMCJITMemoryManager(std::move(MemMgr));
    auto executionEngine = std::unique_ptr<llvm::ExecutionEngine>(factory.create());
    executionEngine->finalizeObject();
    if(entry_point) {
        auto* ep = executionEngine->getPointerToFunction(entry_point);
        std::function<int()> entryPoint = (int(*)())ep;

        //std::cout << "jit begin" << std::endl;
        auto result = entryPoint();
        //std::cout << "jit result: " << result << std::endl;
        return wall_e::right(result);
    } else {
        std::cout << "jit entry point not set" << std::endl;
    }
    return wall_e::left<std::string>("entry point not specified");
}


llvm::LLVMContext* km2::module::context() const {
    return m_context.get();
}



llvm::Module* km2::module::llvmModule() const {
    return m_module.get();
}



llvm::IRBuilder<>* km2::module::builder() const {
    return m_builder.get();
}







