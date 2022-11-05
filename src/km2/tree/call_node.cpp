#include "call_node.h"
#include "namespace_node.h"

#include <wall_e/src/utility/token_tools.h>
#include <iostream>
#include <src/km2/backend/unit/unit.h>
#include <src/km2/backend/unit/capabilities/namespace_capability.h>
#include <src/km2/backend/entities/value.h>
#include "wall_e/src/macro.h"


km2::call_node::call_node(const wall_e::index &index,
        const std::string &name,
        const std::vector<std::shared_ptr<km2::abstract_value_node>>& args,
        const wall_e::text_segment& name_segment
        ) : km2::abstract_value_node(index, cast_to_children(args)) {
    m_name = name;
    m_args = args;
    m_name_segment = name_segment;
}

wall_e::gram::argument km2::call_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
    if(debug) std::cout << "km2::call_node::create" << std::endl;
    if(args.size() > 2) {
        const auto function_name_token = args[0].value<wall_e::lex::token>();
        const auto function_name_end_token = args[1].value<wall_e::lex::token>();
        const auto constrained_args = args[2].constrain();
        const auto function_args = wall_e::remove_tokens(constrained_args, { "EP" });

        std::vector<std::shared_ptr<abstract_value_node>> funcArgs;
        funcArgs.reserve(function_args.size());
        for(const auto& arg : function_args) {
            if (arg.value_or<wall_e::lex::token>().name != "COMA") {
                funcArgs.push_back(arg.cast_or<std::shared_ptr<abstract_value_node>>());
            }
        }

        return std::make_shared<call_node>(
                    index,
                    function_name_token.text,
                    funcArgs,
                    function_name_token.segment()
                    );
    }
    return nullptr;
}


wall_e::either<
    wall_e::error,
    km2::backend::value*
> km2::call_node::generate_backend_value(const std::shared_ptr<km2::backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << "name: " << m_name << std::endl;

    wall_e::list<std::string> namepace_name;
    if(const auto ns = nearest_ancestor<namespace_node>()) {
        namepace_name = ns->full_name();
    }

    if(const auto block_node = nearest_ancestor<km2::block_node>()) {
        if(debug) std::cout << wall_e_this_function << "find: " << namepace_name << "::" << m_name << std::endl;
        if(const auto& overload = block_node->find_overload_in_whole_tree(namepace_name, m_name)) {
            wall_e::vec<backend::value*> args;
            wall_e::vec<backend::type*> arg_types;
            wall_e::vec<wall_e::text_segment> arg_segments;
            args.reserve(m_args.size());
            arg_types.reserve(m_args.size());

            for(const auto& arg : m_args) {
                if (arg) {
                    if(const auto arg_value = arg->generate_backend_value(unit)) {
                        const auto& argr = arg_value.right_value();
                        args.push_back(argr);
                        arg_types.push_back(argr->value_type());
                        arg_segments.push_back(arg->segment());
                    } else {
                        return arg_value.left();
                    }
                } else {
                    args.push_back(nullptr);
                    arg_types.push_back(nullptr);
                    arg_segments.push_back({});
                }
            }

            if(const auto& func = overload->find(arg_types, nullptr)) {
                //unit->setup_insert_point();
                return wall_e::right<backend::value*>(unit->create_call(func, args));
            } else {
                return wall_e::left(wall_e::error(
                                        "function '" + m_name + "' not defined for specific arg types",
                                        wall_e::error::severity::err,
                                        wall_e::error::stage::semantic,
                                        0,
                                        m_name_segment
                                        ));
            }
        } else {
            return wall_e::left(wall_e::error(
                                    "function '" + m_name + "' not defined",
                                    wall_e::error::severity::err,
                                    wall_e::error::stage::semantic,
                                    0,
                                    m_name_segment
                                    ));
        }
    } else {
        return wall_e::left(wall_e::error(
                                "function '" + m_name + "' not defined in block",
                                wall_e::error::severity::err,
                                wall_e::error::stage::semantic,
                                0,
                                m_name_segment
                                ));
    }
}



void km2::call_node::print(size_t level, std::ostream &stream) const {
    stream << std::string(level, ' ') << "{call_node}:" << std::endl;
    stream << std::string(level + 1, ' ') << "name: " << m_name << std::endl;
    for(const auto &arg : m_args) {
        if(arg) {
            arg->print(level + 1, stream);
        } else {
            stream << std::string(level + 1, ' ') << "null arg" << std::endl;
        }
    }
}


wall_e::list<wall_e::error> km2::call_node::errors() const {
    return {};
}

void km2::call_node::short_print(std::ostream &stream) const {
    stream << "call_node { name: " << m_name << " }";
}

km2::ast_token_list km2::call_node::tokens() const {
    return ast_token_list {
        ast_token {
            .type = AstFunction,
            .node_type = wall_e::type_name<call_node>(),
            .hover = "<b>function</b> " + m_name,
            .text = m_name,
            .segment = m_name_segment
        },
    } + tokens_from_node_list(m_args);
}
