#include "call_node.h"
#include "stmt_node.h"
#include "proto_node.h"

#include <iostream>

km2::stmt_node::stmt_node(const wall_e::index &index, std::shared_ptr<abstract_value_node> node)
    : km2::abstract_value_node(index, { node }),
    m_node(node) {}

wall_e::gram::argument km2::stmt_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    if(debug) std::cout << wall_e::type_name<stmt_node>() << "::create: " << args << std::endl;
    if(args.size() > 0) {
        if(debug) std::cout << "km2::cmd_node::create 1: " << args[0] << std::endl;
        const auto node = args[0].option_cast<std::shared_ptr<abstract_value_node>>();
        if(node.has_value()) {
            if(debug) std::cout << "km2::cmd_node::create 2: " << node.value() << std::endl;
            return std::make_shared<stmt_node>(index, node.value());
        }
    }
    return std::make_shared<stmt_node>(index);
}

wall_e::either<wall_e::error, km2::backend::value*> km2::stmt_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    if(debug) std::cout << __PRETTY_FUNCTION__ << std::endl;
    if(m_node) {
        return m_node->generate_backend_value(unit);
    }
    return wall_e::left(wall_e::error("cmd node not exist"));
}


void km2::stmt_node::print(size_t level, std::ostream &stream) const {
    stream << std::string(level, ' ') << "{stmt_node}:" << std::endl;
    if(m_node) {
        m_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "node not exist" << std::endl;
    }
}


wall_e::list<wall_e::error> km2::stmt_node::errors() const {
    return { wall_e::error("err not implemented") };
}

const km2::backend::context &km2::stmt_node::context() const {
    return m_context;
}

void km2::stmt_node::short_print(std::ostream &stream) const {
    stream << "stmt_node { recursive content }";
}
