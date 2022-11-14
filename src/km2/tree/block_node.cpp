#include "block_node.h"
#include "namespace_node.h"
#include "stmt_node.h"
#include  <iostream>
#include "../backend/models/function_ref.h"
#include "wall_e/src/macro.h"
#include "stmt_node.h"

km2::block_node::block_node(
        const wall_e::index &index,
        const std::shared_ptr<stmt_node> &stmt_node,
        const std::shared_ptr<namespace_node> &namespace_node,
        const std::shared_ptr<block_node> &next_node
        )
    : km2::abstract_value_node(index, cast_to_children(std::vector { stmt_node }, std::vector { next_node })),
      m_stmt_node(stmt_node),
      m_namespace_node(namespace_node),
      m_next_node(next_node),
      m_context(stmt_node ? stmt_node->context() : backend::context({})),
      m_acc_context(stmt_node
                    ? (next_node
                       ? stmt_node->context() + next_node->acc_context()
                       : stmt_node->context())
                    : backend::context({})),
      m_mutable_context(backend::context({})) {}

wall_e::gram::argument km2::block_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) {
    if(debug) std::cout << "km2::block_node::create" << std::endl;
    if (args.size() > 0) {
        const auto stmt = args[0].value_or<std::shared_ptr<stmt_node>>(nullptr);
        const auto nspace = args[0].value_or<std::shared_ptr<namespace_node>>(nullptr);
        if (args.size() > 2) {
            const auto next_block = args[2].value_or<std::shared_ptr<block_node>>(nullptr);
            return std::make_shared<block_node>(index, stmt, nspace, next_block);
        } else {
            return std::make_shared<block_node>(index, stmt, nspace, nullptr);
        }
    }
    return std::make_shared<block_node>(index, nullptr, nullptr);
}


wall_e::either<
    wall_e::error,
    km2::backend::value*
> km2::block_node::generate_backend_value(const std::shared_ptr<km2::backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
    if(m_stmt_node) {
        if(const auto cmd_result = m_stmt_node->generate_backend_value(unit)) {
        } else {
            return cmd_result.left();
        }
    }
    if(m_next_node) {
        return m_next_node->generate_backend_value(unit);
    }
    return wall_e::right<backend::value*>(nullptr);
}

wall_e::list<wall_e::error> km2::block_node::errors() const {
    return (m_stmt_node ? m_stmt_node->errors() : wall_e::list<wall_e::error> {})
            .with(m_namespace_node ? m_namespace_node->errors() : wall_e::list<wall_e::error> {})
            .with(m_next_node ? m_next_node->errors() : wall_e::list<wall_e::error> {});
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

std::ostream &km2::block_node::short_print(std::ostream &stream) const {
    return stream << "block_node { recursive content }";
}

wall_e::list<km2::ast_token> km2::block_node::tokens() const {
    return
            (m_stmt_node ? m_stmt_node->tokens() : wall_e::list<ast_token> {}) +
            (m_namespace_node ? m_namespace_node->tokens() : wall_e::list<ast_token> {}) +
            (m_next_node ? m_next_node->tokens() : wall_e::list<ast_token> {});
}


std::ostream &km2::block_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    if(fmt == Simple) {
        stream << std::string(ctx.level(), ' ') << "{block_node}:" << std::endl;
        if(m_stmt_node) {
            m_stmt_node->write(stream, fmt, ctx.new_child("stmt"));
        } else {
            stream << std::string(ctx.level(), ' ') + "stmt not exist" << std::endl;
        }
        if(m_next_node) {
            m_next_node->write(stream, fmt, ctx.new_child(""));
        } else {
            stream << std::string(ctx.level() + 1, ' ') + "next node not exist" << std::endl;
        }
    } else if(fmt == TreeWriter) {
        stream << ctx.node_begin()
               << "block_node { "
               << (m_stmt_node ? std::string() : std::string("no statement") + (m_next_node ? "" : ", "))
               << (m_next_node ? "" : "no next node")
               << " }"
               << ctx.node_end()
               << ctx.edge();

        if(m_stmt_node) {
            m_stmt_node->write(stream, fmt, ctx.new_child("stmt"));
        }
        if(m_namespace_node) {
            m_namespace_node->write(stream, fmt, ctx.new_child("namespace"));
        }
        if(m_next_node) {
            m_next_node->write(stream, fmt, ctx.new_child(""));
        }
    }
    return stream;
}
