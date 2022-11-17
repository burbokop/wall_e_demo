#include "../entities/function.h"
#include "overload.h"
#include "src/km2/backend/models/function_ref.h"
#include <wall_e/src/private/gram_private.h>
#include "../entities/type.h"
#include "../entities/function.h"
#include "../entities/argument.h"

#include <iostream>

km2::backend::overload::overload(const wall_e::str_list &namespace_stack, const std::string &name, const wall_e::list<function *> &values)
    : m_namespace_stack(namespace_stack),
      m_name(name),
      m_values(values) {}

bool km2::backend::overload::is_same_signature(backend::function *f, wall_e::vec<backend::type *> arg_types, backend::type *return_type) {
    const bool arg_count_match = f->is_var_arg() ? f->args().size() <= arg_types.size() : f->args().size() == arg_types.size();
    if((!return_type || f->return_type()->eq(return_type)) && arg_count_match) {
        for(std::size_t i = 0; i < f->args().size(); ++i) {
            if(f->args().at(i)->value_type()->no_eq(arg_types[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}


bool km2::backend::overload::is_same_signature(km2::backend::function *f0, backend::function *f1) {
    if(f0->return_type()->eq(f1->return_type()) && f0->args().size() == f1->args().size()) {
        for(std::size_t i = 0; i < f0->args().size(); ++i) {
            if(f0->args().at(i)->value_type()->no_eq(f1->args().at(i)->value_type())) {
                return false;
            }
        }
        return true;
    }
    return false;
}

std::shared_ptr<km2::backend::overload> km2::backend::overload::shared_from_function(const function_ref& func) {
    return std::make_shared<overload>(
                func.namespace_stack(),
                func.name(),
                wall_e::list<function*> { func.value() }
                );
}

km2::backend::function *km2::backend::overload::find(wall_e::vec<type*> arg_types, type* return_type) {
    for(const auto& v : m_values) {
        if(is_same_signature(v, arg_types, return_type)) {
            return v;
        }
    }
    return nullptr;
}

wall_e::opt<wall_e::error> km2::backend::overload::add_value(backend::function *value, const wall_e::text_segment &segment) {    
    for(const auto& v : m_values) {
        if(is_same_signature(v, value)) {
            return wall_e::error("same function already exists", wall_e::error::Err, wall_e::error::Semantic, 0, segment);
        }
    }
    m_values.push_back(value);
    return std::nullopt;
}

std::ostream &km2::backend::overload::print(std::ostream &stream) const {
    return stream << m_namespace_stack << "::" << m_name;
}

