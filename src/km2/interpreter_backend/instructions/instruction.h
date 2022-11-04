#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>
#include <memory>
#include <limits>
#include <ostream>

#include <wall_e/src/utility/collections.h>

#include <wall_e/src/models/variant.h>

namespace km2 {
namespace interpreter_backend {

class execution_ctx;

class dyn_value {
public:
    typedef std::size_t size_t_ptr;
    static constexpr std::size_t null_size_t_ptr = std::numeric_limits<std::size_t>::max();
private:
    wall_e::variant m_value;
    size_t_ptr m_stack_ptr = null_size_t_ptr;
public:
    const wall_e::variant& value(execution_ctx* ctx) const;
    dyn_value() {}
    explicit dyn_value(const wall_e::variant& value, size_t_ptr stack_ptr)
        : m_value(value),
          m_stack_ptr(stack_ptr) {}

    typedef std::shared_ptr<dyn_value> ptr;
    static inline ptr from_variant(const wall_e::variant &value) {
        return std::make_shared<dyn_value>(value, null_size_t_ptr);
    }
    static inline ptr from_stack_ptr(size_t_ptr stack_ptr) {
        return std::make_shared<dyn_value>(wall_e::variant(), stack_ptr);
    }

    friend inline std::ostream& operator<<(std::ostream& stream, const ptr& ptr) {
        if(ptr) {
            if(ptr->m_stack_ptr != null_size_t_ptr) {
                return stream << "stack_ptr(" << ptr->m_stack_ptr << ")";
            } else {
                return stream << ptr->m_value;
            }
        } else {
            return stream << "nullptr";
        }
    }
};



class instruction {
    execution_ctx* m_ctx;
public:

    struct index {
        static constexpr std::size_t null_offset = std::numeric_limits<std::size_t>::max();
        std::size_t offset = null_offset;
        instruction* ptr = nullptr;
        void eval_ptr(execution_ctx* ctx);
        void force_eval_offset(execution_ctx* ctx);
        void eval_offset(execution_ctx* ctx);

        index next(execution_ctx* ctx) const;
        friend std::ostream& operator <<(std::ostream& stream, const index& i) {
            return stream << "{ offset: " << i.offset
                          << ", ptr: " << i.ptr
                          << " }";
        }
    };

    struct exec_result {
        std::shared_ptr<dyn_value> value;
        index next_inst;
    };
    instruction(execution_ctx* ctx) : m_ctx(ctx) {}

    index next_index() const;

    inline execution_ctx* ctx() const { return m_ctx; };
    virtual exec_result exec(const wall_e::vec<dyn_value::ptr>&) = 0;
    virtual std::ostream& print(std::ostream&) const = 0;
    virtual void link() = 0;
};

}
}

#endif // INSTRUCTION_H
