#include "block_node.h"
#include  <iostream>
#include "../backend/models/function_ref.h"
#include "wall_e/src/macro.h"

km2::block_node::block_node(const wall_e::index &index, const std::shared_ptr<stmt_node> &stmt, const std::shared_ptr<block_node> &next_node)
    : km2::abstract_value_node(index, cast_to_children(std::vector { stmt }, std::vector { next_node })),
      m_stmt(stmt),
      m_next_node(next_node),
      m_context(stmt ? stmt->context() : backend::context({})),
      m_acc_context(stmt
                    ? (next_node
                       ? stmt->context() + next_node->acc_context()
                       : stmt->context())
                    : backend::context({})),
      m_mutable_context(backend::context({})) {}

wall_e::gram::argument km2::block_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    if(debug) std::cout << "km2::block_node::create" << std::endl;
    if (args.size() > 0) {
        const auto cmd = args[0].value_or<std::shared_ptr<stmt_node>>(nullptr);
        if (args.size() > 2) {
            const auto next_block = args[2].value_or<std::shared_ptr<block_node>>(nullptr);
            return std::make_shared<block_node>(index, cmd, next_block);
        } else {
            return std::make_shared<block_node>(index, cmd, nullptr);
        }
    }
    return std::make_shared<block_node>(index, nullptr);
}


wall_e::either<
    wall_e::error,
    km2::backend::value*
> km2::block_node::generate_backend_value(const std::shared_ptr<km2::backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
    if(m_stmt) {
        if(const auto cmd_result = m_stmt->generate_backend_value(unit)) {
        } else {
            return cmd_result.left();
        }
    }
    if(m_next_node) {
        return m_next_node->generate_backend_value(unit);
    }
    return wall_e::right<backend::value*>(nullptr);
}


void km2::block_node::print(size_t level, std::ostream &stream) const {
    stream << std::string(level, ' ') << "{block_node}:" << std::endl;
    if(m_stmt) {
        m_stmt->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "cmd not exist" << std::endl;
    }
    if(m_next_node) {
        m_next_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "next node not exist" << std::endl;
    }
}


wall_e::list<wall_e::error> km2::block_node::errors() const {
    return { wall_e::error("err not implemented") };
}


std::optional<wall_e::error> km2::block_node::add_function(const backend::function_ref &function) {
    if(const auto& overload = find_overload_in_block_seq(function.namespace_stack(), function.name())) {
        if(const auto error = overload->add_value(function.value(), segment())) {
            return error;
        }
    } else {
        if(const auto error = mutable_context().add_overload(function, segment())) {
            return error;
        }
    }
    return std::nullopt;
}

std::shared_ptr<km2::backend::overload> km2::block_node::find_overload_in_block_seq(const std::list<std::string> &namespace_stack, const std::string &name) {
    if(const auto& overload = mutable_context().find_overload(namespace_stack, name)) {
        return overload;
    } else if(const auto& p = parent_as<block_node>()) {
        return p->find_overload_in_block_seq(namespace_stack, name);
    } else {
        return nullptr;
    }
}

struct __find_overload_in_whole_tree {
    const std::string name;
    const std::list<std::string> namespace_stack;

    std::shared_ptr<km2::backend::overload> __rec(km2::abstract_node* node) const {
        if(auto block_node = dynamic_cast<km2::block_node*>(node)) {
            if(const auto& overload = block_node->mutable_context().find_overload(namespace_stack, name)) {
                return overload;
            }
        }
        if(const auto& p = node->parent()) {
            return __rec(p);
        }
        return nullptr;
    }
};

std::shared_ptr<km2::backend::overload> km2::block_node::find_overload_in_whole_tree(
        const std::list<std::string> &namespace_stack,
        const std::string &name
        ) {
    return __find_overload_in_whole_tree { .name = name, .namespace_stack = namespace_stack }.__rec(this);
}

void km2::block_node::short_print(std::ostream &stream) const {
    stream << "block_node { recursive content }";
}

wall_e::list<km2::ast_token> km2::block_node::tokens() const {
    return
            (m_stmt ? m_stmt->tokens() : wall_e::list<ast_token> {}) +
            (m_next_node ? m_next_node->tokens() : wall_e::list<ast_token> {});
}
