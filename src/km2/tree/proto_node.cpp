#include "decl_arg_node.h"
#include "namespace_node.h"
#include "proto_node.h"
#include <wall_e/src/utility/token_tools.h>

#include <iostream>

#include <src/km2/backend/unit/unit.h>
#include "../backend/models/function_ref.h"
#include "wall_e/src/macro.h"

km2::proto_node::proto_node(const wall_e::index& index,
        const std::string &name,
        const wall_e::text_segment& name_segment,
        const wall_e::vec<std::shared_ptr<decl_arg_node> > &args,
        std::shared_ptr<abstract_type_node> result_type_node
        )
    : km2::abstract_value_node(index, cast_to_children(args, std::vector { result_type_node })),
    m_name(name),
    m_name_segment(name_segment),
    m_args(args),
    m_result_type_node(result_type_node) {}

wall_e::gram::argument km2::proto_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    if(debug) std::cout << "km2::proto_node::create: " << args << std::endl;

    if(args.size() > 4 && args[0].contains_type<wall_e::lex::token>()) {
        wall_e::vec<std::shared_ptr<decl_arg_node>> da_nodes;
        const auto decl_args = args[3].constrain();
        for(const auto& decl_arg : decl_args) {
            const auto da_node = decl_arg.option_cast<std::shared_ptr<km2::decl_arg_node>>();
            if(da_node.has_value()) {
                da_nodes.push_back(da_node.value());
            }
        }

        return std::make_shared<proto_node>(
                index,
                args[0].value<wall_e::lex::token>().text,
                args[0].value<wall_e::lex::token>().segment(),
                da_nodes,
                args[4].cast_or<std::shared_ptr<abstract_type_node>>()
                );
    }
    return nullptr;
}

wall_e::either<
    wall_e::error,
    km2::backend::value*
> km2::proto_node::generate_backend_value(const std::shared_ptr<km2::backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
    wall_e::vec<backend::type*> arg_types;
    bool is_var_arg = false;
    for(const auto& arg : m_args) {
        if(arg->is_variadic()) {
            if(is_var_arg) {
                std::cerr << "... repeaded while function declaration\n";
            }
            is_var_arg = true;
        } else {
            if(const auto type = arg->type_node()->generate_backend_type(unit)) {
                arg_types.push_back(type.right_value());
            } else {
                return type.left();
            }
        }
    }

    if(m_result_type_node) {
        if (const auto rt = m_result_type_node->generate_backend_type(unit)) {
            const auto& proto = unit->proto(m_name, arg_types, rt.right_value(), is_var_arg);

            wall_e::str_list namepace_name;
            if(const auto& ns = nearest_ancestor<namespace_node>()) {
                namepace_name = ns->full_name();
            }

            if(const auto& block_node = nearest_ancestor<km2::block_node>()) {
                if(const auto& error = block_node->add_function(backend::function_ref(namepace_name, m_name, proto))) {
                    return wall_e::left(*error);
                }
            }

            return wall_e::right<backend::value*>(proto);
        } else {
            return rt.left();
        }
    } else {
        return wall_e::left(wall_e::error("result type node missing"));
    }
}

wall_e::list<wall_e::error> km2::proto_node::errors() const {
    return { wall_e::error("err not implemented") };
}

std::ostream &km2::proto_node::short_print(std::ostream &stream) const {
    return stream << "proto_node: { name: " << m_name << " }";
}


km2::ast_token_list km2::proto_node::tokens() const {
    using namespace km2::literals;
    return ast_token_list {
        ast_token {
            .type = AstFunction,
            .modifier = AstDefinition,
            .node_type = wall_e::type_name<proto_node>(),
            .hover = "**function** "_md + m_name + " (prototype)",
            .text = m_name,
            .segment = m_name_segment
        },
    } + tokens_from_node_list(m_args)
    + (m_result_type_node ? m_result_type_node->tokens() : ast_token_list {});
}


std::ostream &km2::proto_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    if(fmt == Simple) {
        stream << std::string(ctx.level(), ' ') << "{proto_node}:" << std::endl;
        stream << std::string(ctx.level() + 1, ' ') << "name: " << m_name << std::endl;
        for(const auto& arg : m_args) {
            if(arg) {
                arg->write(stream, fmt, ctx.new_child("arg"));
            } else {
                stream << std::string(ctx.level() + 1, ' ') << "arg node missing" << std::endl;
            }
        }
        if(m_result_type_node) {
            m_result_type_node->write(stream, fmt, ctx.new_child("result type"));
        } else {
            stream << std::string(ctx.level() + 1, ' ') << "result type node missing" << std::endl;
        }
    } else if(fmt == TreeWriter) {
        stream << ctx.node_begin()
               << "proto_node { name: " << m_name << (m_result_type_node ? "" : ", no result type node") << " }"
               << ctx.node_end()
               << ctx.edge();

        for(const auto& arg : m_args) {
            const auto child_ctx = ctx.new_child("arg");
            if(arg) {
                arg->write(stream, fmt, child_ctx);
            } else {
                stream << child_ctx.node_begin()
                       << "[null arg]"
                       << child_ctx.node_end()
                       << child_ctx.edge();
            }
        }
        if(m_result_type_node) {
            m_result_type_node->write(stream, fmt, ctx.new_child("result type"));
        }
    }
    return stream;
}
