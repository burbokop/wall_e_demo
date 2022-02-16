#include "context.h"

km2::context::context() {}

std::list<std::shared_ptr<km2::overload> > km2::context::overloads() const { return m_overloads; }

std::optional<wall_e::error> km2::context::add_overload(const std::shared_ptr<overload> &overload, const wall_e::text_segment &segment) {
    if(find_overload(overload->namespace_stack(), overload->name())) {
        return wall_e::error("overload already exist", wall_e::error::err, wall_e::error::semantic, 0, segment);
    } else {
        m_overloads.push_back(overload);
        return std::nullopt;
    }
}

std::optional<wall_e::error> km2::context::add_overload(const function &func, const wall_e::text_segment &segment) {
    return add_overload(overload::shared_from_function(func), segment);
}

std::shared_ptr<km2::overload> km2::context::find_overload(
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

km2::context km2::context::operator +(const context &other) const {
    return km2::context();
}


