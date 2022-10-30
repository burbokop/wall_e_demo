#ifndef MODULE_H
#define MODULE_H


#pragma warning(push, 0)
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#pragma warning(pop)

#include "capabilities/abstract_capability.h"
#include <wall_e/src/models/either.h>
#include <wall_e/src/lex.h>

#include <set>

namespace km2 {

class translation_unit {
    std::unique_ptr<llvm::LLVMContext> m_context;
    std::unique_ptr<llvm::IRBuilder<>> m_builder;
    std::unique_ptr<llvm::Module> m_module;

    struct ctx {
        llvm::BasicBlock* block = nullptr;
        std::map<std::string, llvm::Value*> args;
    };
    std::stack<ctx> m_stack;


    mutable std::map<std::size_t, abstract_capability* /* deleting with unit */> m_capabilities;
public:
    enum ArgSettingStatus {
        ArgSettingSuccess,
        ArgSettingDublicates,
        ArgSettingEmptyStack
    };

    translation_unit();
    ~translation_unit();

    template<typename T>
    T* cap() const {
        static_assert (std::is_base_of<abstract_capability, T>::value, "T must be inherited from km2::abstract_capability");
        const auto& hash = typeid (T).hash_code();
        const auto it = m_capabilities.find(hash);
        if(it == m_capabilities.end()) {
            const auto& result = new T(const_cast<translation_unit*>(this));
            m_capabilities.insert(std::pair<std::size_t, abstract_capability*> { hash, result });
            assert(result->unit());
            return result;
        } else {
            return dynamic_cast<T*>(it->second);
        }
    }

    llvm::Value* execute(const std::function<llvm::Value*(llvm::LLVMContext*, llvm::IRBuilder<>*, llvm::Module*)>& func);
    ArgSettingStatus set_arg(const std::string& name, llvm::Value* value);
    llvm::Value* arg(const std::string& name) const;
    llvm::FunctionType *func(llvm::Type* returnType, llvm::ArrayRef<llvm::Type*> argTypes);
    llvm::BasicBlock *begin_block(const std::string& name, llvm::Function *func = nullptr, const std::vector<std::string>& argNames = {});
    void end_block();
    void setup_insert_point();
    llvm::Function* begin_entry(const std::string& name);


    /**
     * @brief proto function creates function proto with unmangled symbol
     * @param name - name of symbol
     * @param arg_types - types of arguments
     * @param result_type - type of result
     * @param is_var_arg - is C varargs
     * @return llvm value
     */
    llvm::Function* proto(
            const std::string& name,
            std::vector<llvm::Type*> arg_types,
            llvm::Type* result_type,
            bool is_var_arg = false
            );

    llvm::CallInst *inline_asm(const std::string &text);




    void print();
    void print_functions();
    std::string llvm_assembly() const;
    wall_e::either<std::string, int> compile(const std::string &output_path);
    wall_e::either<std::string, int> run_jit(llvm::Function* entry_point);

    /**
     * @brief make_executable
     * @param output_path
     * @return
     * @note do not call in thread and dont ask why
     */
    wall_e::either<std::string, int> make_executable(const std::string &output_path);

    llvm::LLVMContext *llvm_context() const;
    llvm::Module* llvm_module() const;
    llvm::IRBuilder<> *llvm_builder() const;

    [[deprecated("dev function. will be removed")]]
    int gen();
    [[deprecated("dev function. will be removed")]]
    llvm::Function* createSumFunction();
    [[deprecated("dev function. will be removed")]]
    int aaa();
};

}

#endif // MODULE_H
