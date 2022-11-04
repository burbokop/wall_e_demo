#include "symbol.h"

#include <wall_e/src/lex.h>

std::string km2::backend::symbol::cppmangle(const proto &p) {
    return "_Z"
            + (p.namespace_name.size() > 0 ? "N" + wall_e::lex::join(p.namespace_name, "", [](const std::string& part){
        return std::to_string(part.size()) + part;
    }) : "")
            + p.name
            + "v"; // TODO add arg types mangle;
}

km2::backend::symbol::symbol(
        const std::list<std::string> &namespace_name,
        const std::string &name,
        const std::list<std::string> &arg_types,
        const std::string &ret_type,
        const std::function<std::string (const proto &)> &mangle_func
        ) :
    m_namespace_name(namespace_name),
    m_name(name),
    m_arg_types(arg_types),
    m_ret_type(ret_type),
    m_mangle_func(mangle_func),
    m_mangled_name(mangle_func ? mangle_func(proto{
        .namespace_name = namespace_name,
        .name = name,
        .arg_types = arg_types,
        .ret_type = ret_type
    }) : std::string()) {}

std::list<std::string> km2::backend::symbol::namespace_name() const { return m_namespace_name; }
std::string km2::backend::symbol::name() const { return m_name; }
std::list<std::string> km2::backend::symbol::arg_types() const { return m_arg_types; }
std::function<std::string(const km2::backend::symbol::proto&)> km2::backend::symbol::mangle_func() const { return m_mangle_func; }
std::string km2::backend::symbol::mangled_name() const { return m_mangled_name; }
std::string km2::backend::symbol::ret_type() const { return m_ret_type; }
