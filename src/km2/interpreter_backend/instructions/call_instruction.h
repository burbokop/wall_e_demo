#ifndef CALL_INSTRUCTION_H
#define CALL_INSTRUCTION_H

#include "instruction.h"
#include "../entities/function.h"

namespace km2 {
namespace interpreter_backend {

class dyn_function {
    mutable instruction::index m_entry;
public:
    typedef std::function<dyn_value::ptr(execution_ctx*, const wall_e::vec<dyn_value::ptr>&)> native_func;
private:
    native_func m_native;
public:
    dyn_function(instruction::index entry) : m_entry(entry) {}
    dyn_function(native_func native) : m_native(native) {}
    void force_eval_offset(execution_ctx* ctx) { m_entry.force_eval_offset(ctx); }
    const instruction::index &entry(execution_ctx* ctx) const;

    const native_func& native() { return m_native; }

    inline bool is_native() const { return m_native ? true : false; }

    std::ostream& print(std::ostream& stream) const;
};

class native_function_provider {
public:
    virtual dyn_function::native_func native_func(const std::string& name) const = 0;
    virtual ~native_function_provider() {}
};

typedef std::shared_ptr<dyn_function> dyn_function_ptr;

class call_instruction : public instruction {
    dyn_function_ptr m_f;
    wall_e::vec<dyn_value::ptr> m_args;
public:
    call_instruction(
            execution_ctx* ctx,
            dyn_function_ptr f,
            const wall_e::vec<dyn_value::ptr>& args
            )
        : instruction(ctx),
          m_f(f),
          m_args(args) {};

    // instruction interface
public:
    virtual exec_result exec(const wall_e::vec<dyn_value::ptr>&) override;
    virtual std::ostream &print(std::ostream &) const override;
    virtual void link() override { m_f->force_eval_offset(ctx()); }
};


class ret_instruction : public instruction {

public:
    using instruction::instruction;

    // instruction interface
public:
    virtual exec_result exec(const wall_e::vec<dyn_value::ptr> &) override;
    virtual std::ostream &print(std::ostream &) const override;
    virtual void link() override {}
};

}
}

#endif // CALL_INSTRUCTION_H
