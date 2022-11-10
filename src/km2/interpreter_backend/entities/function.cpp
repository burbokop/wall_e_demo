#include "function.h"

#include "argument.h"
#include "type.h"
#include "block.h"
#include "../instructions/call_instruction.h"

wall_e::box_list<km2::interpreter_backend::argument> km2::interpreter_backend::function::born_args(
        const wall_e::vec<type *> &arg_types,
        function *f
        ) {
    wall_e::box_list<argument> result;
    //result.reserve(arg_types.size());
    for(std::size_t i = 0; i < arg_types.size(); ++i) {
        //type* type, function* func, const std::shared_ptr<dyn_value>& dyn
        result.make(arg_types[i], f, i);
    }
    return result;
}

std::shared_ptr<km2::interpreter_backend::dyn_function> km2::interpreter_backend::function::dyn() const {
    if(!m_dyn) {
        if(const auto& b = m_blocks.front_opt()) {
            if(const auto& i = (*b)->instructions().front_opt()) {
                m_dyn = std::make_shared<dyn_function>(instruction::index {
                    .offset = null_size_t_ptr,
                    .ptr = *i
                });
            }
        } else if(m_native_function_provider) {
            if(const auto& nf = m_native_function_provider->native_func(name())) {
                m_dyn = std::make_shared<dyn_function>(nf);
            }
        }
    }
    return m_dyn;
}

km2::backend::type *km2::interpreter_backend::function::value_type() const {
    return nullptr;
}

bool km2::interpreter_backend::function::is_var_arg() const {
    return m_is_var_arg;
}

km2::backend::type *km2::interpreter_backend::function::return_type() {
    return m_return_type;
}

std::string km2::interpreter_backend::function::name() const {
    return m_name;
}

wall_e::vec<km2::backend::argument*> km2::interpreter_backend::function::args() const {
    wall_e::vec<km2::backend::argument*> result;
    result.reserve(m_args.size());
    for(const auto& a : m_args) {
        result.push_back(dynamic_cast<km2::backend::argument*>(a));
    }
    return result;
}

wall_e::list<km2::backend::block*> km2::interpreter_backend::function::blocks() {
    return m_blocks.map<km2::backend::block*>(block::upcast);
}

std::ostream &km2::interpreter_backend::function::print(std::ostream &stream) const {
    return stream << wall_e::type_name<function>() << " "
                  << "{ name: " << m_name
                  << ", return_type: " << m_return_type
                  << ", args: " << args()
                  << ", is_var_arg: " << m_is_var_arg
                  << ", blocks: " << m_blocks
                  << " }";
}

