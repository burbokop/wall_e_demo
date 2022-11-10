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

class call_stack {
    std::size_t m_offset = 0;
    struct frame {
        typedef std::map<std::string, km2::backend::value*> arguments;
        interpreter_backend::block* block;
        arguments args;
        std::size_t offset;
    };
    std::stack<frame> m_stack;
public:
    call_stack() {}
    bool empty() const { return m_stack.empty(); }
    const frame& top() const { return m_stack.top(); }
    frame::arguments& top_args() { return m_stack.top().args; }
    void push_frame(
            interpreter_backend::block* block,
            const std::map<std::string, km2::backend::value*>& args
            ) {
        const auto delta = args.size() + 1;
        m_stack.push(frame { .block = block, .args = args, .offset = delta });
        m_offset += delta;
    }
    void pop_frame() {
        m_offset -= m_stack.top().offset;
        m_stack.pop();
    }
    std::size_t offset() const { return m_offset; }

    void push(std::size_t n = 1) {
        m_stack.top().offset += n;
        m_offset += n;
    }

};

class unit : public km2::backend::unit {
    wall_e::box_list<block> m_blocks;
    wall_e::box_list<function> m_functions;
    wall_e::box_list<constant> m_values;

    std::shared_ptr<execution_ctx> m_ctx;

    call_stack m_stack;

    block* born_block(
            execution_ctx* ctx,
            const std::string& name,
            function* f,
            std::size_t stack_offset
            );
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
    virtual wall_e::either<std::string, int> run_jit(km2::backend::function* entry_point, bool verbose = false) override;

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
