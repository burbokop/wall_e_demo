#include "decl_arg_node.h"
#include "function_node.h"
#include "namespace_node.h"


#include <wall_e/src/utility/defer.h>
#include <wall_e/src/utility/token_tools.h>
#include <iostream>
#include <src/km2/backend/unit/unit.h>
#include <src/km2/backend/unit/capabilities/type_capability.h>
#include <src/km2/backend/models/function_ref.h>

wall_e::gram::argument km2::function_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    if(debug) std::cout << "km2::function_node::create: " << args << std::endl;
    if(args.size() > 5 && args[0].contains_type<wall_e::lex::token>()) {
        wall_e::vec<std::shared_ptr<decl_arg_node>> da_nodes;
        const auto decl_args = args[3].constrain();
        for(const auto& decl_arg : decl_args) {
            const auto da_node = decl_arg.option_cast<std::shared_ptr<km2::decl_arg_node>>();
            if(da_node.has_value()) {
                da_nodes.push_back(da_node.value());
            }
        }

        return std::make_shared<function_node>(
                index,
                args[0].value<wall_e::lex::token>().text,
                args[0].value<wall_e::lex::token>().segment(),
                da_nodes,
                args[5].cast_or<std::shared_ptr<abstract_value_node>>()
                );
    }
    return nullptr;
}

km2::function_node::function_node(
        const wall_e::index &index,
        const std::string &name,
        const wall_e::text_segment &name_segment,
        const wall_e::vec<std::shared_ptr<decl_arg_node> > &args,
        std::shared_ptr<abstract_value_node> body
        )
    : abstract_value_node(index, cast_to_children(args, std::vector { body })),
      m_name(name),
      m_name_segment(name_segment),
      m_args(args),
      m_body(body) {}


wall_e::either<
    wall_e::error,
    km2::backend::value *
> km2::function_node::generate_backend_value(const std::shared_ptr<km2::backend::unit> &unit) {
    if(debug) std::cout << __PRETTY_FUNCTION__ << std::endl;
    wall_e::vec<backend::type*> arg_types;
    wall_e::vec<std::string> arg_names;
    for(const auto& arg : m_args) {
        if(const auto type = arg->type_node()->generate_backend_type(unit)) {
            arg_types.push_back(type.right().value());
            arg_names.push_back(arg->name());
        } else {
            return type.left();
        }
    }

    wall_e::str_list namepace_name;
    if(const auto ns = nearest_ancestor<namespace_node>()) {
        namepace_name = ns->full_name();
    }

    const auto proto = unit->proto(m_name, arg_types, unit->cap<backend::type_capability>()->void_type());

    const auto block = unit->begin_block(m_name + "_block", proto, arg_names);
    wall_e::defer end_block_defer([unit, block]{
        if(block) {
            unit->create_void_return();
            unit->end_block();
        }
    });

    if(const auto block_node = nearest_ancestor<km2::block_node>()) {
        if(const auto& error = block_node->add_function(backend::function_ref(namepace_name, m_name, proto))) {
            return wall_e::left(*error);
        }
    }

    if (m_body) {
        if(const auto body = m_body->generate_backend_value(unit)) {

        } else  {
            return body.left();
        }
    }

    return wall_e::right<backend::value*>(proto);
}

void km2::function_node::print(size_t level, std::ostream &stream) const {
    stream << std::string(level, ' ') << "{function_node}:" << std::endl;
    if(const auto ns = nearest_ancestor<namespace_node>()) {
        stream << std::string(level + 1, ' ') << "name: " << wall_e::lex::join(ns->full_name(), "::") << "::" << m_name << std::endl;
    } else {
        stream << std::string(level + 1, ' ') << "name: " << m_name << std::endl;
    }
    stream << std::string(level + 1, ' ') << "args: " << std::endl;

    for(const auto& a : m_args) {
        if(a) {
            a->print(level + 1, stream);
        } else {
            stream << std::string(level + 1, ' ') + "null arg" << std::endl;
        }
    }
    stream << std::string(level + 1, ' ') << "body: " << std::endl;
    if(m_body) {
        m_body->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') + "body not exist" << std::endl;
    }
}

wall_e::list<wall_e::error> km2::function_node::errors() const {
    return {};
}

void km2::function_node::short_print(std::ostream &stream) const {
    stream << "function_node { name: " << m_name << ", args: " << m_args << " }";
}

km2::ast_token_list km2::function_node::tokens() const {
    return ast_token_list {
        km2::ast_token {
            .type = AstFunction,
            .node_type = wall_e::type_name<function_node>(),
            .hover = "<b>function</b> " + m_name,
            .text = m_name,
            .segment = m_name_segment
        },
    } + tokens_from_node_list(m_args)
    + (m_body ? m_body->tokens() : ast_token_list {});
}
