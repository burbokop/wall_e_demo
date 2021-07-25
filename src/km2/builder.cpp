#include "builder.h"

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

km2::module_builder::module_builder() {
    m_context = std::make_unique<llvm::LLVMContext>();
    m_builder = std::unique_ptr<llvm::IRBuilder<>>(new llvm::IRBuilder<>(*m_context));
    m_module = std::make_unique<llvm::Module>("Module", *m_context);

}

llvm::Value *km2::module_builder::execute(const std::function<llvm::Value *(llvm::LLVMContext *, llvm::IRBuilder<> *, llvm::Module *)> &func) {
    return func(m_context.get(), m_builder.get(), m_module.get());
}

llvm::ConstantInt *km2::module_builder::uint32(uint32_t value) {
    return llvm::ConstantInt::getSigned((llvm::Type::getInt32Ty(*m_context)), value);
}

llvm::ConstantInt *km2::module_builder::uintptr(uintptr_t value) {
    return llvm::ConstantInt::getSigned(m_builder->getIntPtrTy(m_module->getDataLayout()), value);
}

llvm::FunctionType *km2::module_builder::func(llvm::Type* returnType, llvm::ArrayRef<llvm::Type*> argTypes) {
    return llvm::FunctionType::get(returnType, argTypes, false);
}

llvm::Value *km2::module_builder::format(const std::string &name, const std::string &text) {
    const auto format_str = llvm::ConstantDataArray::getString(*m_context.get(), text, true);

    const auto global_format_str = new llvm::GlobalVariable(
                *m_module.get(),
                format_str->getType(),
                true,
                llvm::GlobalValue::ExternalLinkage,
                format_str,
                name
                );

    return m_builder->CreateGEP(global_format_str, uintptr(0));
}

llvm::Type *km2::module_builder::type(const std::string &name) {
    std::regex reg("^u([0-9]+)$");
    std::smatch matches;
    if(std::regex_search(name, matches, reg)) {
        if (matches.size() > 1) {
            const auto base = matches[1];
            try {
                return llvm::Type::getIntNTy(*m_context.get(), std::stoi(base));
            }  catch (std::exception e) {

            }
        }
    }
    return nullptr;
}

llvm::Value *km2::module_builder::value(const wall_e::lex::token &token) {

}

llvm::BasicBlock *km2::module_builder::beginBlock(const std::string &name, llvm::Function *func) {
    llvm::BasicBlock *entryBasicBlock = llvm::BasicBlock::Create(*m_context.get(), name, func);
    m_builder->SetInsertPoint(entryBasicBlock);
    return entryBasicBlock;
}

int km2::module_builder::gen() {
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

llvm::Function *km2::module_builder::createSumFunction() {
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

int km2::module_builder::aaa() {
    auto sumFunc = createSumFunction();


    m_entryPoint = llvm::Function::Create(
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

    const auto format_str_ptr = format(".fmt", "Hello World: %d\n");

    m_builder->CreateCall(printf_func, {
                              format_str_ptr,
                              a
                          });





    m_builder->CreateRet(a);

    print();

    return 0;
}

llvm::Function *km2::module_builder::proto(llvm::Type* resultType, std::vector<llvm::Type*> argTypes, const std::string& name) {
    return llvm::Function::Create(
                llvm::FunctionType::get(
                    resultType,
                    argTypes,
                    false
                    ),
                llvm::Function::ExternalLinkage,
                name,
                m_module.get()
                );
}

llvm::CallInst *km2::module_builder::inline_asm(const std::string& text) {

    llvm::InlineAsm *IA = llvm::InlineAsm::get(nullptr, text, "", true, false);

    llvm::CallInst *result = m_builder->CreateCall(IA, {});

    //result->addAttribute(llvm::AttributeSet::FunctionIndex, llvm::Attribute::NoUnwind);
    return result;
}

void km2::module_builder::print() {
    llvm::outs() << llvm::raw_ostream::CYAN << "\nMODULE:\n" << llvm::raw_ostream::GREEN << *m_module.get() << llvm::raw_ostream::RESET;
}

int km2::module_builder::runJit() {
    std::cout << "\nRUN JIT\n";
    llvm::TargetOptions Opts;
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    std::unique_ptr<llvm::RTDyldMemoryManager> MemMgr(new llvm::SectionMemoryManager());

    llvm::EngineBuilder factory(std::move(m_module));
    factory.setEngineKind(llvm::EngineKind::JIT);
    factory.setTargetOptions(Opts);
    factory.setMCJITMemoryManager(std::move(MemMgr));
    auto executionEngine = std::unique_ptr<llvm::ExecutionEngine>(factory.create());
    //m_module.get()->setDataLayout(executionEngine->getDataLayout());
    executionEngine->finalizeObject();

    if(m_entryPoint) {
        auto* ep = executionEngine->getPointerToFunction(m_entryPoint);
        std::function<int()> entryPoint = (int(*)())ep;

        std::cout << "jit begin" << std::endl;
        auto r = entryPoint();
        std::cout << "jit result: " << r << std::endl;
        return r;
    }
    return -2;
}


llvm::LLVMContext* km2::module_builder::context() const {
    return m_context.get();
}



llvm::Module* km2::module_builder::module() const {
    return m_module.get();
}



llvm::IRBuilder<>* km2::module_builder::builder() const {
    return m_builder.get();
}
