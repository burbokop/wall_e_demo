#ifndef DYN_VALUE_H
#define DYN_VALUE_H

#include <wall_e/src/models/variant.h>
#include <limits>
#include "instruction.h"
#include "size_t_ptr.h"
#include <variant>

namespace km2 {
namespace interpreter_backend {

class execution_ctx;

class dyn_value {
    typedef std::variant<
        wall_e::variant,
        size_t_ptr,
        instruction::index
    > data;

    data m_data;
    const wall_e::variant& value(execution_ctx* ctx, const wall_e::opt<size_t_ptr>& first_stack_ptr) const;
public:
    /** DO NOT USE */ explicit dyn_value(const data& data) : m_data(data) {}
    dyn_value() : m_data(wall_e::variant()) {}

    typedef std::shared_ptr<dyn_value> ptr;
    static inline ptr from_variant(const wall_e::variant &value) {
        return std::make_shared<dyn_value>(data(value));
    }
    static inline ptr from_stack_ptr(size_t_ptr stack_ptr) {
        return std::make_shared<dyn_value>(data(stack_ptr));
    }
    static inline ptr from_instruction_index(const instruction::index& idx) {
        return std::make_shared<dyn_value>(data(idx));
    }

    inline bool is_var() const { return m_data.index() == 0; }
    inline bool is_stack_ptr() const { return m_data.index() == 1; }
    inline bool is_instruction_index() const { return m_data.index() == 2; }

    inline const auto& var() const { return std::get<0>(m_data); }
    inline const auto& stack_ptr() const { return std::get<1>(m_data); }
    inline const auto& instruction_index() const { return std::get<2>(m_data); }

    const wall_e::variant& value(execution_ctx* ctx) const {
        if(is_var() || is_stack_ptr()) {
            wall_e::opt<size_t_ptr> sp;
            if(is_stack_ptr()) {
                sp = stack_ptr();
            }
            return value(ctx, sp);
        } else {
            static const wall_e::variant v;
            return v;
        }
    }

    inline bool points_to(size_t_ptr stack_ptr) const {
        if(is_stack_ptr()) {
            return this->stack_ptr() == stack_ptr
                    && this->stack_ptr() != null_size_t_ptr;
        } else {
            return false;
        }
    }


    friend inline std::ostream& operator<<(std::ostream& stream, const ptr& ptr) {
        if(ptr) {
            if(ptr->is_var()) {
                return stream << ptr->var()
                                 .map<std::string, std::string>(wall_e::lex::encode_special_syms);
            } else if(ptr->is_stack_ptr()) {
                return stream << ptr->stack_ptr() << "_s";
            } else if(ptr->is_instruction_index()) {
                return stream << ptr->instruction_index() << "_i";
            } else {
                return stream << "undef var";
            }
        } else {
            return stream << "nullptr";
        }
    }
};

}
}

#endif // DYN_VALUE_H
