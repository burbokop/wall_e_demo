#include "call_node.h"
#include "stmt_node.h"
#include "proto_node.h"

#include "wall_e/src/macro.h"
#include <iostream>

km2::stmt_node::stmt_node(const wall_e::index &index, std::shared_ptr<abstract_value_node> node)
    : km2::abstract_value_node(index, { node }),
    m_node(node) {}

wall_e::gram::argument km2::stmt_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    if(debug) std::cout << wall_e::type_name<stmt_node>() << "::create: " << args << std::endl;
    if(args.size() > 0) {
        if(debug) std::cout << "km2::cmd_node::create 1: " << args[0] << std::endl;
        std::cout << "args[0]: " << args[0] << std::endl;
        std::cout << "args[0].type: " << args[0].type() << std::endl;
        std::cout << "args[0].lineage: " << args[0].lineage() << std::endl;
        const auto node = args[0].option_cast<std::shared_ptr<abstract_value_node>>();
        if(node.has_value()) {
            if(debug) std::cout << "km2::cmd_node::create 2: " << node.value() << std::endl;
            return std::make_shared<stmt_node>(index, node.value());
        }
    }
    return std::make_shared<stmt_node>(index);
}

wall_e::either<wall_e::error, km2::backend::value*> km2::stmt_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
    if(m_node) {
        return m_node->generate_backend_value(unit);
    }
    return wall_e::left(wall_e::error("cmd node not exist"));
}

wall_e::list<wall_e::error> km2::stmt_node::errors() const {
    return { wall_e::error("err not implemented") };
}

const km2::backend::context &km2::stmt_node::context() const {
    return m_context;
}

std::ostream &km2::stmt_node::short_print(std::ostream &stream) const {
    return stream << "stmt_node { recursive content }";
}


std::ostream &km2::stmt_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    if(fmt == Simple) {
        stream << std::string(ctx.level(), ' ') << "{stmt_node}:" << std::endl;
        if(m_node) {
            m_node->write(stream, fmt, ctx.new_child(""));
        } else {
            stream << std::string(ctx.level() + 1, ' ') + "node not exist" << std::endl;
        }
    } else if(fmt == TreeWriter) {
        stream << ctx.node_begin()
               << "stmt_node: { " << (m_node ? "" : "no node") << " }"
               << ctx.node_end()
               << ctx.edge();
        if(m_node) {
            m_node->write(stream, fmt, ctx.new_child(""));
        }
    }
    return stream;
}
