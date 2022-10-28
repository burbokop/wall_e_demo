#include "decl_arg_node.h"


bool km2::decl_arg_node::is_variadic() const {
    return m_is_variadic;
}

std::shared_ptr<km2::abstract_type_node> km2::decl_arg_node::type_node() const {
    return m_type_node;
}

std::string km2::decl_arg_node::name() const {
    return m_name;
}

km2::decl_arg_node::decl_arg_node(const wall_e::index &index, const std::string &name, std::shared_ptr<abstract_type_node> type_node, bool is_variadic)
    : km2::abstract_node(index, { type_node }),
    m_name(name),
    m_type_node(type_node),
    m_is_variadic(is_variadic)
{}

wall_e::gram::argument km2::decl_arg_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    if(args.size() > 1) {
        if(args[0].contains_type<wall_e::lex::token>()) {
            const auto id = args[0].value<wall_e::lex::token>().text;
            if(args[1].value_or<wall_e::lex::token>().name == "THREE_DOT") {
                return std::make_shared<decl_arg_node>(index, id, nullptr, true);
            } else {
                return std::make_shared<decl_arg_node>(index, id, args[1].cast_or<std::shared_ptr<abstract_type_node>>(), false);
            }
        }
    }
    return std::make_shared<decl_arg_node>(index, std::string(), nullptr, false);
}


void km2::decl_arg_node::print(size_t level, std::ostream &stream) const {
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

void km2::decl_arg_node::short_print(std::ostream &stream) const {
    stream << "decl_arg_node { name: " << m_name << ", is_variadic: " << m_is_variadic << " }";
}


std::list<wall_e::error> km2::decl_arg_node::errors() const {
    return {};
}
