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


    //struct stack_frame {
    //    instruction::index ret;
    //    wall_e::vec<dyn_value::ptr> values;
    //    inline friend std::ostream& operator<<(std::ostream& stream, const stack_frame& sf) {
    //        return stream << "{ ret: " << sf.ret
    //                      << ", values: " << sf.values
    //                      << " }";
    //    }
    //};
private:
    wall_e::vec<std::shared_ptr<dyn_value>> m_stack;

    wall_e::list<std::string> m_errs;

    bool m_verbose = false;
    bool m_should_exit = false;
public:
    execution_ctx();
    void set_verbose(bool v) { m_verbose = v; }
    bool verbose() const { return m_verbose; }
    void add_err(const std::string& err) { m_errs.push_back(err); }
    void fault();
    void append_instructions(const wall_e::vec<instruction*>& i) { m_instructions.append(i); }
    void clear_instructions() { m_instructions.clear(); }
    const wall_e::vec<instruction*>& instructions() const { return m_instructions; };
    instruction::index current_index() const { return m_current_index; };

    inline std::shared_ptr<dyn_value> stack_top() const {
        if(m_stack.empty()) return nullptr;
        return m_stack.back();
    }

    inline void stack_push(const std::shared_ptr<dyn_value>& v) { m_stack.push_back(v); }
    inline void stack_push(const wall_e::vec<std::shared_ptr<dyn_value>>& v) {
        m_stack.insert(m_stack.end(), v.begin(), v.end());
    }



    std::shared_ptr<dyn_value> try_dereference(size_t_ptr ptr);

    inline std::shared_ptr<dyn_value> stack_pop() {
        if(m_stack.empty()) return nullptr;
        auto t = m_stack.back();
        m_stack.pop_back();
        return t;
    }

    inline std::shared_ptr<dyn_value> stack_pop(std::size_t count) {
        if(count == 0) return nullptr;
        if(m_stack.size() < count) {
            add_err("can not pop. count > stack size");
            fault();
            return nullptr;
        }
        m_stack.resize(m_stack.size() - count + 1);
        return stack_pop();
    }

    std::ostream& print(std::ostream&) const;

    bool proceed_next();

    void exec(const  dyn_function_ptr& entry);
    void exec(const instruction::index& entry);

    const wall_e::list<std::string> &errs() const { return m_errs; }

    // native_function_provider interface
public:
    virtual dyn_function::native_func native_func(const std::string &name) const override;
};

}
}

#endif // EXECUTION_CTX_H
