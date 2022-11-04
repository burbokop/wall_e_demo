#ifndef KM2_INTERPRETER_BACKEND_UNIT_H
#define KM2_INTERPRETER_BACKEND_UNIT_H

#include <wall_e/src/models/either.h>
#include <wall_e/src/lex.h>
#include <set>
#include "../../backend/unit/unit.h"
#include <wall_e/src/utility/box.h>



namespace km2 {

namespace interpreter_backend {

class execution_ctx;
class instruction;
class block;
class function;
class value;
class type;
class constant;
class argument;
class native_function_provider;
class dyn_value;

class unit : public km2::backend::unit {
    wall_e::box_list<block> m_blocks;
    wall_e::box_list<function> m_functions;
    wall_e::box_list<constant> m_values;

    std::shared_ptr<execution_ctx> m_ctx;

    struct ctx {
        interpreter_backend::block* block;
        std::map<std::string, km2::backend::value*> args;
        std::size_t offset = 0;
    };
    std::stack<ctx> m_stack;

    interpreter_backend::block* m_insert_point = nullptr;

    block* born_block(execution_ctx* ctx, const std::string& name, function* f);
    function* born_function(
            const std::string& name,
            type* return_type,
            const wall_e::vec<type*>& arg_type,
            bool is_var_arg,
            const native_function_provider* native_function_provider
            );
    constant* born_constant(type* type, const std::shared_ptr<dyn_value>& dyn);
public:


    unit(const km2::backend::backend* b);

    //virtual backend::value* execute(const std::function<backend::value*(llvm::LLVMContext*, llvm::IRBuilder<>*, llvm::Module*)>& func) override;
    virtual ArgSettingStatus set_arg(const std::string& name, km2::backend::value* value) override;
    virtual km2::backend::value* arg(const std::string& name) const override;

    virtual km2::backend::block *begin_block(const std::string& name, km2::backend::function *func, const wall_e::str_vec& arg_names) override;

    virtual void end_block() override;
    void setup_insert_point() /** should not be virtual in case of error with 'fn2' */;
    virtual km2::backend::function* begin_entry(const std::string& name) override;

    /**
     * @brief proto function creates function proto with unmangled symbol
     * @param name - name of symbol
     * @param arg_types - types of arguments
     * @param result_type - type of result
     * @param is_var_arg - is C varargs
     * @return llvm value
     */
    virtual km2::backend::function* proto(
            const std::string& name,
            wall_e::vec<km2::backend::type*> arg_types,
            km2::backend::type* result_type,
            bool is_var_arg = false
            ) override;

    virtual void create_void_return() override;
    virtual void create_return(km2::backend::value *val) override;
    virtual km2::backend::value *create_call(km2::backend::function *func, const wall_e::vec<km2::backend::value *> &args) override;

    void link();


    virtual /**llvm::CallInst*/ void *inline_asm(const std::string &text) override;

    virtual void print() override;
    virtual void print_functions() override;
    virtual std::string llvm_assembly() const override;
    virtual wall_e::either<std::string, int> compile(const std::string &output_path) override;
    virtual wall_e::either<std::string, int> run_jit(km2::backend::function* entry_point) override;

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
    virtual km2::backend::function* create_sum_function() override;
    [[deprecated("dev function. will be removed")]]
    virtual int aaa() override;

};

}

}

#endif // MODULE_H
