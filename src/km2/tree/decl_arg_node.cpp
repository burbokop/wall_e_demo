#include "decl_arg_node.h"

namespace km2 {

bool decl_arg_node::is_variadic() const
{
    return m_is_variadic;
}

abstract_type_node *decl_arg_node::type_node() const
{
    return m_type_node;
}

const std::string &decl_arg_node::name() const
{
    return m_name;
}

decl_arg_node::decl_arg_node(const std::string &name, abstract_type_node* type_node, bool is_variadic) {
    m_name = name;
    m_type_node = type_node;
    m_is_variadic = is_variadic;
}

wall_e::gram::argument decl_arg_node::create(const wall_e::gram::arg_vector &args) {
    if(args.size() > 1) {
        if(args[0].contains_type<wall_e::lex::token>()) {
            const auto id = args[0].value<wall_e::lex::token>().text;
            if(args[1].value_default<wall_e::lex::token>().name == "THREE_DOT") {
                return new decl_arg_node(id, nullptr, true);
            } else {
                return new decl_arg_node(id, args[1].default_cast<km2::abstract_type_node*>(), false);
            }
        }
    }
    return new decl_arg_node("", nullptr, false);
}

} // namespace km2


void km2::decl_arg_node::print(size_t level, std::ostream &stream) {
    stream << std::string(level, ' ') << "{decl_arg_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "name: " << m_name << std::endl;
    if(m_type_node) {
        m_type_node->print(level + 1, stream);
    } else if(m_is_variadic) {
        stream << std::string(level + 1, ' ') << "is variadic" << std::endl;
    } else {
        stream << std::string(level + 1, ' ') << "not variadic & type missing" << std::endl;
    }
}
