#ifndef BUILDER_H
#define BUILDER_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <wall_e/src/lex.h>

namespace km2 {

class module_builder {
    std::unique_ptr<llvm::LLVMContext> m_context;
    std::unique_ptr<llvm::IRBuilder<>> m_builder;
    std::unique_ptr<llvm::Module> m_module;
    llvm::Function* m_entryPoint = nullptr;

    struct ctx {
        llvm::BasicBlock* block = nullptr;
        std::map<std::string, llvm::Value*> args;
    };
    std::stack<ctx> m_stack;
public:
    module_builder();

    llvm::Value* execute(const std::function<llvm::Value*(llvm::LLVMContext*, llvm::IRBuilder<>*, llvm::Module*)>& func);

    llvm::ConstantInt *uint32(uint32_t value);
    llvm::ConstantInt *uintptr(uintptr_t value);
    llvm::ConstantInt *uint(uint64_t value, size_t size);

    llvm::Constant *float64(double value);

    llvm::Value* arg(const std::string& name) const;

    llvm::FunctionType *func(llvm::Type* returnType, llvm::ArrayRef<llvm::Type*> argTypes);
    llvm::Value *string_const_ptr(const std::string &name, const std::string& text);

    llvm::BasicBlock *beginBlock(const std::string& name, llvm::Function *func = nullptr, const std::vector<std::string>& argNames = {});

    void endBlock();
    void setupInsertPoint();

    llvm::Function* beginEntry(const std::string& name);

    int gen();
    llvm::Function* createSumFunction();
    int aaa();

    llvm::Function *proto(llvm::Type* resultType, std::vector<llvm::Type*> argTypes, const std::string& name, bool isVarArg = false);
    llvm::CallInst *inline_asm(const std::string &text);

    void print();
    int runJit();

    llvm::LLVMContext* context() const;
    llvm::Module*module() const;
    llvm::IRBuilder<>*builder() const;
};

}

#endif // BUILDER_H
