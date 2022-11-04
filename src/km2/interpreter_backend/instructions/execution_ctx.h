#ifndef EXECUTION_CTX_H
#define EXECUTION_CTX_H

#include "instruction.h"
#include <wall_e/src/utility/collections.h>
#include <stack>
#include "call_instruction.h"

namespace km2 {
namespace interpreter_backend {

class execution_ctx : public /** impl */ native_function_provider {
    wall_e::vec<instruction*> m_instructions;
    instruction::index m_current_index;
public:
    struct stack_frame {
        instruction::index ret;
        wall_e::vec<dyn_value::ptr> values;
        inline friend std::ostream& operator<<(std::ostream& stream, const stack_frame& sf) {
            return stream << "{ ret: " << sf.ret
                          << ", values: " << sf.values
                          << " }";
        }
    };
private:
    wall_e::stack<stack_frame> m_call_stack;

    std::list<std::string> m_errs;

    bool m_verbose = false;
public:
    execution_ctx();
    void set_verbose(bool v) { m_verbose = v; }
    bool verbose() const { return m_verbose; }
    void add_err(const std::string& err) { m_errs.push_back(err); }
    void fault();
    void append_instructions(const wall_e::vec<instruction*>& i) { m_instructions.append(i); }
    void clear_instructions() { m_instructions.clear(); }
    const wall_e::vec<instruction*>& instructions() const { return m_instructions; };
    //inline wall_e::opt<instruction*> instruction_at(instruction::index i) const {
    //    if(i.offset < m_instructions.size()) {
    //        return m_instructions[i.offset];
    //    } else {
    //        return std::nullopt;
    //    }
    //};
    instruction::index current_index() const { return m_current_index; };

    inline wall_e::opt<stack_frame> call_stack_top() const {
        if(m_call_stack.empty()) return std::nullopt;
        return m_call_stack.top();
    }

    inline void call_stack_push(const stack_frame& sf) { m_call_stack.push(sf); }
    inline wall_e::opt<stack_frame> call_stack_pop() {
        if(m_call_stack.empty()) return std::nullopt;
        auto t = m_call_stack.top();
        m_call_stack.pop();
        return t;
    }

    std::ostream& print(std::ostream&) const;

    bool proceed_next();

    void exec(const  dyn_function_ptr& entry);
    void exec(const instruction::index& entry);

    const std::list<std::string> &errs() const { return m_errs; }

    // native_function_provider interface
public:
    virtual dyn_function::native_func native_func(const std::string &name) const override;
};

}
}

#endif // EXECUTION_CTX_H
