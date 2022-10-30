#include "decl_arg_node.h"
#include "namespace_node.h"
#include "proto_node.h"
#include <wall_e/src/utility/token_tools.h>

#include <iostream>

#include <src/km2/translation_unit/translation_unit.h>


km2::proto_node::proto_node(
        const wall_e::index& index,
        const std::string &name,
        const wall_e::text_segment& name_segment,
        const std::vector<std::shared_ptr<decl_arg_node>> &args,
        std::shared_ptr<abstract_type_node> result_type_node
        )
    : km2::abstract_value_node(index, cast_to_children(args, std::vector { result_type_node })),
    m_name(name),
    m_name_segment(name_segment),
    m_args(args),
    m_result_type_node(result_type_node) {}

wall_e::gram::argument km2::proto_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    std::cout << "km2::proto_node::create: " << args << std::endl;

    if(args.size() > 4 && args[0].contains_type<wall_e::lex::token>()) {
        std::vector<std::shared_ptr<decl_arg_node>> da_nodes;
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
    llvm::Value *
> km2::proto_node::generate_llvm(const std::shared_ptr<km2::translation_unit> &unit) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::vector<llvm::Type*> argTypes;
    bool isVarArg = false;
    for(const auto& arg : m_args) {
        if(arg->is_variadic()) {
            if(isVarArg) {
                std::cerr << "... repeaded while function declaration\n";
            }
            isVarArg = true;
        } else {
            if(const auto type = arg->type_node()->generate_llvm(unit)) {
                argTypes.push_back(type.right_value());
            } else {
                return type.left();
            }
        }
    }

    if(m_result_type_node) {
        if (const auto rt = m_result_type_node->generate_llvm(unit)) {
            const auto& proto = unit->proto(m_name, argTypes, rt.right_value(), isVarArg);

            std::list<std::string> namepace_name;
            if(const auto& ns = nearest_ancestor<namespace_node>()) {
                namepace_name = ns->full_name();
            }

            if(const auto& block_node = nearest_ancestor<km2::block_node>()) {
                if(const auto& error = block_node->add_function(km2::function(namepace_name, m_name, proto))) {
                    return wall_e::left(*error);
                }
            }

            return wall_e::right<llvm::Value *>(proto);
        } else {
            return rt.left();
        }
    } else {
        return wall_e::left(wall_e::error("result type node missing"));
    }
}

void km2::proto_node::print(size_t level, std::ostream &stream) const {
    stream << std::string(level, ' ') << "{proto_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "name: " << m_name << std::endl;
    for(const auto& arg : m_args) {
        if(arg) {
            arg->print(level + 1, stream);
        } else {
            stream << std::string(level + 1, ' ') << "arg node missing" << std::endl;
        }
    }
    if(m_result_type_node) {
        m_result_type_node->print(level + 1, stream);
    } else {
        stream << std::string(level + 1, ' ') << "result type node missing" << std::endl;
    }
}


wall_e::list<wall_e::error> km2::proto_node::errors() const {
    return { wall_e::error("err not implemented") };
}

void km2::proto_node::short_print(std::ostream &stream) const {
    stream << "proto_node: { name: " << m_name << " }";
}


km2::ast_token_list km2::proto_node::tokens() const {
    return ast_token_list {
        ast_token {
            .type = AstFunction,
            .node_type = wall_e::type_name<proto_node>(),
            .hover = "<b>function</b> " + m_name + " (prototype)",
            .text = m_name,
            .segment = m_name_segment
        },
    } + tokens_from_node_list(m_args)
    + (m_result_type_node ? m_result_type_node->tokens() : ast_token_list {});
}
