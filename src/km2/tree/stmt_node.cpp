#include "call_node.h"
#include "stmt_node.h"
#include "proto_node.h"

#include "wall_e/src/macro.h"
#include <iostream>

km2::stmt_node::stmt_node(
        const wall_e::gram::environment *env,
        const wall_e::index &index,
        std::shared_ptr<abstract_value_node> node
        )
    : km2::abstract_value_node(env, index, { node }),
    m_node(node) {}

wall_e::gram::argument km2::stmt_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) {
    if(env->verbose()) std::cout << wall_e::type_name<stmt_node>() << "::create: " << args << std::endl;
    if(args.size() > 0) {
        if(const auto node = args[0].option_cast<std::shared_ptr<abstract_value_node>>()) {
            return std::make_shared<stmt_node>(env, index, *node);
        }
    }
    return std::make_shared<stmt_node>(env, index, nullptr);
}

wall_e::either<wall_e::error, km2::backend::value*> km2::stmt_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    if(env()->verbose()) std::cout << wall_e_this_function << std::endl;
    if(m_node) {
        return m_node->generate_backend_value(unit);
    }
    return wall_e::left(wall_e::error("cmd node not exist"));
}

wall_e::list<wall_e::error> km2::stmt_node::errors() const {
    return m_node ? m_node->errors() : wall_e::list<wall_e::error> {};
}

const km2::backend::context &km2::stmt_node::context() const {
    return m_context;
}

std::ostream &km2::stmt_node::short_print(std::ostream &stream) const {
    return stream << "stmt_node { recursive content }";
}


std::ostream &km2::stmt_node::write(std::ostream &stream, const wall_e::tree_writer::context &ctx) const {
    stream << ctx.node_begin()
           << "stmt_node: { " << (m_node ? "" : "no node") << " }"
           << ctx.node_end()
           << ctx.edge();
    if(m_node) {
        m_node->write(stream, ctx.new_child(""));
    }
    return stream;
}

km2::ast_token_type km2::stmt_node::rvalue_type() const {
    return m_node ? m_node->rvalue_type() : wall_e::enums::null;
}

km2::markup_string km2::stmt_node::hover() const {
    return m_node ? m_node->hover() : km2::markup_string();
}
