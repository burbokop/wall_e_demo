#include "call_node.h"
#include "namespace_node.h"

#include <wall_e/src/utility/token_tools.h>
#include <iostream>
#include <src/km2/backend/unit/unit.h>
#include <src/km2/backend/unit/capabilities/namespace_capability.h>
#include <src/km2/backend/entities/value.h>
#include "wall_e/src/macro.h"


km2::call_node::call_node(
        const wall_e::index &index,
        const wall_e::lex::token_vec& namespace_stack,
        const std::string &name,
        const wall_e::vec<std::shared_ptr<abstract_value_node> > &args,
        const wall_e::text_segment& name_segment
        )
    : km2::abstract_value_node(index, cast_to_children(args)),
      m_namespace_stack(namespace_stack),
      m_name(name),
      m_args(args),
      m_name_segment(name_segment) {}

wall_e::gram::argument km2::call_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) {
    if(debug) std::cout << "km2::call_node::create: " << args << std::endl;
    if(args.size() > 3) {
        const auto namespaces = args[0].constrain();
        const auto function_name_token = args[1].option<wall_e::lex::token>();
        const auto function_name_end_token = args[2].option<wall_e::lex::token>();
        if(function_name_token && function_name_end_token) {
            const auto constrained_args = args[3].constrain();
            const auto function_args = wall_e::remove_tokens(constrained_args, { "EP" });

            const auto& namespace_stack = namespaces
                    .filter_map<wall_e::lex::token>([](const wall_e::variant& n) -> wall_e::opt<wall_e::lex::token> {
                if(const auto& nt = n.option<wall_e::lex::token>()) {
                    if(nt->name == "ID") {
                        return nt;
                    }
                }
                return std::nullopt;
            });

            wall_e::vec<std::shared_ptr<abstract_value_node>> funcArgs;
            funcArgs.reserve(function_args.size());
            for(const auto& arg : function_args) {
                if (arg.value_or<wall_e::lex::token>().name != "COMA") {
                    funcArgs.push_back(arg.cast_or<std::shared_ptr<abstract_value_node>>());
                }
            }

            return std::make_shared<call_node>(
                        index,
                        namespace_stack,
                        function_name_token->text,
                        funcArgs,
                        function_name_token->segment()
                        );
        }
    }
    return std::make_shared<call_node>(index, wall_e::lex::token_vec {}, std::string(), wall_e::vec<std::shared_ptr<km2::abstract_value_node>>(), wall_e::text_segment());
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
                                        wall_e::error::severity::Err,
                                        wall_e::error::stage::Semantic,
                                        0,
                                        m_name_segment
                                        ));
            }
        } else {
            return wall_e::left(wall_e::error(
                                    "function '" + m_name + "' not defined",
                                    wall_e::error::severity::Err,
                                    wall_e::error::stage::Semantic,
                                    0,
                                    m_name_segment
                                    ));
        }
    } else {
        return wall_e::left(wall_e::error(
                                "function '" + m_name + "' not defined in block",
                                wall_e::error::severity::Err,
                                wall_e::error::stage::Semantic,
                                0,
                                m_name_segment
                                ));
    }
}

wall_e::list<wall_e::error> km2::call_node::errors() const {
    return m_args
            .map<wall_e::list<wall_e::error>>([](const std::shared_ptr<abstract_value_node>& arg){ return arg ? arg->errors() : wall_e::list<wall_e::error> {}; })
            .reduce({}, [](const auto& a, const auto& b) { return a.with(b); });
}

std::ostream &km2::call_node::short_print(std::ostream &stream) const {
    return stream << "call_node { name: " << m_name << " }";
}

km2::ast_token_list km2::call_node::tokens() const {
    using namespace km2::literals;

    ast_token_list namespace_tokens;
    for(const auto &n : m_namespace_stack) {
        namespace_tokens.push_back(ast_token {
                                       .type = AstNamespace,
                                       .modifier = wall_e::enums::max_value<ast_token_modifier>(),
                                       .node_type = wall_e::type_name<call_node>(),
                                       .hover = "**namespace** "_md + n.text,
                                       .text = n.text,
                                       .segment = n.segment()
                                   });
    }

    return std::move(namespace_tokens) + ast_token_list {
        ast_token {
            .type = AstFunction,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<call_node>(),
            .hover = hover(),
            .text = m_name,
            .segment = m_name_segment
        },
    } + tokens_from_node_list(m_args);
}


std::ostream &km2::call_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    std::string full_name;
    if(m_namespace_stack.size() > 0) {
        full_name = m_namespace_stack.map_member<std::string>(&wall_e::lex::token::text).join("::") + "::" + m_name;
    } else {
        full_name = m_name;
    }

    if(fmt == Simple) {
        stream << std::string(ctx.level(), ' ') << "{call_node}:" << std::endl;
        stream << std::string(ctx.level() + 1, ' ') << "name: " << m_name << std::endl;
        for(const auto &arg : m_args) {
            if(arg) {
                arg->write(stream, fmt, ctx.new_child("arg"));
            } else {
                stream << std::string(ctx.level() + 1, ' ') << "null arg" << std::endl;
            }
        }
    } else if(fmt == TreeWriter) {
        stream << ctx.node_begin()
               << "call_node: { " << full_name << " }"
               << ctx.node_end()
               << ctx.edge();

        for(const auto &arg : m_args) {
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
    }
    return stream;
}

km2::ast_token_type km2::call_node::rvalue_type() const {
    return AstVariable; /// because function returns variable
}

km2::markup_string km2::call_node::hover() const {
    using namespace km2::literals;
    return "**function** "_md + m_name;
}
