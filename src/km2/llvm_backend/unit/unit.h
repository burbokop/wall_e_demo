#ifndef KM2_LLVM_BACKEND_MODULE_H
#define KM2_LLVM_BACKEND_MODULE_H

#include <wall_e/src/models/either.h>
#include <wall_e/src/lex.h>
#include <set>
#include "../../backend/unit/unit.h"

namespace km2 {

namespace llvm_backend {

class unit_private;

class unit : backend::unit {    
    std::unique_ptr<unit_private> m_p;
public:
    inline unit_private* p() const { return m_p.get(); };

    unit(backend::backend* b);

    //virtual backend::value* execute(const std::function<backend::value*(llvm::LLVMContext*, llvm::IRBuilder<>*, llvm::Module*)>& func) override;
    virtual ArgSettingStatus set_arg(const std::string& name, backend::value* value) override;
    virtual backend::value* arg(const std::string& name) const override;
    virtual backend::block *begin_block(const std::string& name, backend::function *func = nullptr, const wall_e::str_vec& arg_names = {}) override;
    virtual void end_block() override;
    virtual void setup_insert_point() override;
    virtual backend::function* begin_entry(const std::string& name) override;

    /**
     * @brief proto function creates function proto with unmangled symbol
     * @param name - name of symbol
     * @param arg_types - types of arguments
     * @param result_type - type of result
     * @param is_var_arg - is C varargs
     * @return llvm value
     */
    virtual backend::function* proto(
            const std::string& name,
            wall_e::vec<backend::type*> arg_types,
            backend::type* result_type,
            bool is_var_arg = false
            ) override;

    virtual /**llvm::CallInst*/ void *inline_asm(const std::string &text) override;

    virtual void print() override;
    virtual void print_functions() override;
    virtual std::string llvm_assembly() const override;
    virtual wall_e::either<std::string, int> compile(const std::string &output_path) override;
    virtual wall_e::either<std::string, int> run_jit(backend::function* entry_point) override;

    /**
     * @brief make_executable
     * @param output_path
     * @return
     * @note do not call in thread and dont ask why
     */
    virtual wall_e::either<std::string, int> make_executable(const std::string &output_path) override;

    //virtual llvm::LLVMContext *llvm_context() const override;
    //virtual llvm::Module* llvm_module() const override;
    //virtual llvm::IRBuilder<> *llvm_builder() const override;

    [[deprecated("dev function. will be removed")]]
    virtual int gen() override;
    [[deprecated("dev function. will be removed")]]
    virtual backend::function* create_sum_function() override;
    [[deprecated("dev function. will be removed")]]
    virtual int aaa() override;
};

}

}

#endif // MODULE_H
