#include "block_node.h"
#include "namespace_node.h"
#include "stmt_node.h"
#include  <iostream>
#include "../backend/models/function_ref.h"
#include "wall_e/src/macro.h"
#include "stmt_node.h"

km2::block_node::block_node(
        const wall_e::index &index,
        const wall_e::vec<std::shared_ptr<km2::stmt_node>> &statements
        )
    : km2::abstract_value_node(index, cast_to_children(statements)),
      m_statements(statements),
      m_context(/*statements ? statements->context() :*/ backend::context({})),
      m_acc_context(/*statements
                    ? (next_node
                       ? statements->context() + next_node->acc_context()
                       : statements->context())
                    : */backend::context({})),
      m_mutable_context(backend::context({})) {}


wall_e::gram::argument km2::block_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) {
    if(debug) std::cout << "km2::block_node::create: " << args << std::endl;
    const auto statements = wall_e::gram::argument(args).constrain().filter_map<std::shared_ptr<stmt_node>>([](const wall_e::gram::argument& a){
        return a.option_cast<std::shared_ptr<stmt_node>>();
    });
    return std::make_shared<block_node>(index, statements);
}


wall_e::either<
    wall_e::error,
    km2::backend::value*
> km2::block_node::generate_backend_value(const std::shared_ptr<km2::backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
    /** TODO
    if(m_stmt_node) {
        if(const auto cmd_result = m_stmt_node->generate_backend_value(unit)) {
        } else {
            return cmd_result.left();
        }
    }
    if(m_next_node) {
        return m_next_node->generate_backend_value(unit);
    }
    */
    return wall_e::right<backend::value*>(nullptr);
}

wall_e::list<wall_e::error> km2::block_node::errors() const {
    return m_statements
            .map<wall_e::list<wall_e::error>>([](const std::shared_ptr<stmt_node> n) { return n->errors(); })
            .reduce({}, &wall_e::list<wall_e::error>::with_op);
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

std::shared_ptr<km2::backend::overload> km2::block_node::find_overload_in_block_seq(const std::list<std::string> &namespace_stack, const std::string &name) const {
    if(const auto& overload = const_cast<block_node*>(this)->mutable_context().find_overload(namespace_stack, name)) {
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

    std::shared_ptr<km2::backend::overload> __rec(const km2::abstract_node* node) const {
        if(auto block_node = dynamic_cast<const km2::block_node*>(node)) {
            //if(const auto& overload = block_node->mutable_context().find_overload(namespace_stack, name)) {
            //    return overload;
            //}
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
        ) const {
    return __find_overload_in_whole_tree { .name = name, .namespace_stack = namespace_stack }.__rec(this);
}


struct __find_call_candidate_nodes {
    const wall_e::str_list &namespace_stack;
    const std::string &name;
    const km2::block_node::search_scope &search_scope;

    wall_e::list<std::shared_ptr<km2::abstract_value_node>> __rec(const km2::abstract_node* node) const {
        wall_e::list<std::shared_ptr<km2::abstract_value_node>> result;



        //if(auto block_node = dynamic_cast<km2::block_node*>(node)) {
        //    if(const auto& overload = block_node->mutable_context().find_overload(namespace_stack, name)) {
        //        return overload;
        //    }
        //}
        //if(const auto& p = node->parent()) {
        //    return __rec(p);
        //}
        //return nullptr;
    }
};

wall_e::list<std::shared_ptr<km2::abstract_value_node>> km2::block_node::find_call_candidate_nodes(
        const wall_e::str_list &namespace_stack,
        const std::string &name,
        search_scope search_scope
        ) const {
    return __find_call_candidate_nodes { .namespace_stack = namespace_stack, .name = name, .search_scope = search_scope }.__rec(this);
}

std::ostream &km2::block_node::short_print(std::ostream &stream) const {
    return stream << "block_node { recursive content }";
}

wall_e::list<km2::ast_token> km2::block_node::tokens() const {
    return m_statements
            .map<wall_e::list<ast_token>>([](const std::shared_ptr<stmt_node> n) { return n->tokens(); })
            .reduce({}, &wall_e::list<ast_token>::with_op);
}


std::ostream &km2::block_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    if(fmt == Simple) {
        stream << std::string(ctx.level(), ' ') << "{block_node}:" << std::endl;
        //if(m_stmt_node) {
        //    m_stmt_node->write(stream, fmt, ctx.new_child("stmt"));
        //} else {
        //    stream << std::string(ctx.level(), ' ') + "stmt not exist" << std::endl;
        //}
        //if(m_next_node) {
        //    m_next_node->write(stream, fmt, ctx.new_child(""));
        //} else {
        //    stream << std::string(ctx.level() + 1, ' ') + "next node not exist" << std::endl;
        //}
    } else if(fmt == TreeWriter) {
        stream << ctx.node_begin()
               << "block_node"
               << ctx.node_end()
               << ctx.edge();

        for(const auto &statement : m_statements) {
            const auto child_ctx = ctx.new_child("stmt");
            if(statement) {
                statement->write(stream, fmt, child_ctx);
            } else {
                stream << child_ctx.node_begin()
                       << "[null statement]"
                       << child_ctx.node_end()
                       << child_ctx.edge();
            }
        }
    }
    return stream;
}

km2::ast_token_type km2::block_node::rvalue_type() const {
    return wall_e::enums::null;
}

km2::markup_string km2::block_node::hover() const {
    return km2::markup_string();
}
