#include "context.h"
#include <wall_e/src/private/gram_private.h>

km2::backend::context::context() {}

std::list<std::shared_ptr<km2::backend::overload>> km2::backend::context::overloads() const { return m_overloads; }

std::optional<wall_e::error> km2::backend::context::add_overload(const std::shared_ptr<overload> &overload, const wall_e::text_segment &segment) {
    if(find_overload(overload->namespace_stack(), overload->name())) {
        return wall_e::error("overload already exist", wall_e::error::Err, wall_e::error::Semantic, 0, segment);
    } else {
        m_overloads.push_back(overload);
        return std::nullopt;
    }
}

std::optional<wall_e::error> km2::backend::context::add_overload(const function_ref &func, const wall_e::text_segment &segment) {
    return add_overload(overload::shared_from_function(func), segment);
}

std::shared_ptr<km2::backend::overload> km2::backend::context::find_overload(
        const std::list<std::string> &namespace_stack,
        const std::string &name
        ) {
    for(auto& o : m_overloads) {
        if(o->name() == name && o->namespace_stack() == namespace_stack) {
            return o;
        }
    }
    return nullptr;
}

km2::backend::context km2::backend::context::operator +(const context &other) const {
    return km2::backend::context();
}

std::ostream &km2::backend::context::print(std::ostream &stream) const {
    return stream << "context " << m_overloads;
}


