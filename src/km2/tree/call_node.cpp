#include "call_node.h"
#include "imp_node.h"
#include "namespace_node.h"

#include <wall_e/src/utility/token_tools.h>
#include <iostream>
#include <src/km2/backend/unit/unit.h>
#include <src/km2/backend/unit/capabilities/namespace_capability.h>
#include <src/km2/backend/entities/value.h>
#include <src/km2/tree/traits/callable_trait.h>
#include "wall_e/src/macro.h"
#include "function_node.h"

/// TODO make comparison not between strings but between lvalues
static wall_e::str_list remap_name_from_module(const wall_e::str_list& input, const km2::imp_node* imp) {
    auto it = input.begin();
    for(; it != input.end(); ++it) {
        if(*it == "<export>") {
            break;
        }
    }

    if(it != input.end()) ++it;

    //std::cout << "remap_name_from_module " << imp->lval_full_name() << " + " <<  input << " (" << wall_e::str_list(it, input.end()) << ")" << std::endl;
    return imp->lval_full_name() + wall_e::str_list(it, input.end());
}

std::shared_ptr<const km2::abstract_value_node> km2::call_node::eval_declaration() const {
    return root()->find<abstract_value_node, imp_node, pub_api_searcher<imp_node>>([this](
                                                         const std::shared_ptr<const abstract_value_node>& f,
                                                         const imp_node* imp
                                                         ) -> bool {
        if(f->has<callable_trait>()) {
            const auto& im = dynamic_cast<const imp_node*>(imp);
            const auto& remapped_name = im ? remap_name_from_module(f->lval_full_name(), im) : f->lval_full_name();
            const auto& s = subtract_namespace_stack(remapped_name, current_nspace_stack());
            //std::cout << "serach func from " << current_nspace_stack() << ": " << f->lval_full_name() << " -> " << remapped_name << " (" << s << ")" << " == " << full_apeal_name() << std::endl;
            return s == full_apeal_name();
        } else {
            return false;
        }
    });
}

wall_e::str_list km2::call_node::full_apeal_name() const {
    const auto n = m_apeal_namespace_stack.map<std::string>([](const wall_e::lex::token& t) { return t.text; });
    return wall_e::str_list(n.begin(), n.end()) + m_name;
}

wall_e::str_list km2::call_node::current_nspace_stack() const {
    if(const auto ns = nearest_ancestor<namespace_node>()) {
        return ns->full_name();
    }
    return {};
}

km2::call_node::call_node(
        const wall_e::gram::environment* env,
        const wall_e::index &index,
        const wall_e::lex::token_vec& apeal_namespace_stack,
        const std::string &name,
        const wall_e::vec<std::shared_ptr<abstract_value_node> > &args,
        const wall_e::text_segment& name_segment
        )
    : km2::abstract_value_node(env, index, cast_to_children(args)),
      m_apeal_namespace_stack(apeal_namespace_stack),
      m_name(name),
      m_args(args),
      m_name_segment(name_segment) {}

wall_e::gram::argument km2::call_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) {
    if(env->verbose()) std::cout << "km2::call_node::create: " << args << std::endl;
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
                        env,
                        index,
                        namespace_stack,
                        function_name_token->text,
                        funcArgs,
                        function_name_token->segment()
                        );
        }
    }
    return std::make_shared<call_node>(env, index, wall_e::lex::token_vec {}, std::string(), wall_e::vec<std::shared_ptr<km2::abstract_value_node>>(), wall_e::text_segment());
}

wall_e::either<
    wall_e::error,
    km2::backend::value*
> km2::call_node::generate_backend_value(const std::shared_ptr<km2::backend::unit> &unit) {
    if(env()->verbose()) std::cout << wall_e_this_function << "name: " << m_name << std::endl;

    wall_e::list<std::string> namepace_name;
    if(const auto ns = nearest_ancestor<namespace_node>()) {
        namepace_name = ns->full_name();
    }

    if(const auto block_node = nearest_ancestor<km2::block_node>()) {
        if(env()->verbose()) std::cout << wall_e_this_function << "find: " << namepace_name << "::" << m_name << std::endl;
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
    const auto children_errs = m_args
            .map<wall_e::list<wall_e::error>>([](const std::shared_ptr<abstract_value_node>& arg){ return arg ? arg->errors() : wall_e::list<wall_e::error> {}; })
            .reduce({}, [](const auto& a, const auto& b) { return a.with(b); });

    if(!declaration()) {
        return wall_e::list {
            wall_e::error(
                        "function '" + full_apeal_name().join("::") + "' not found in this scope",
                        wall_e::error::Err,
                        wall_e::error::Semantic,
                        semantic_error::FunctionNotFound,
                        m_name_segment
                        )
        } + children_errs;
    }
    return children_errs;
}

std::ostream &km2::call_node::short_print(std::ostream &stream) const {
    return stream << "call_node { name: " << m_name << " }";
}

km2::ast_token_list km2::call_node::tokens() const {
    using namespace km2::literals;

    ast_token_list namespace_tokens;
    for(const auto &n : m_apeal_namespace_stack) {
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
            .type = declaration() ? declaration()->rvalue_type() : AstFunction,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<call_node>(),
            .hover = hover(),
            .text = m_name,
            .segment = m_name_segment
        },
    } + tokens_from_node_list(m_args);
}


std::ostream &km2::call_node::write(std::ostream &stream, const wall_e::tree_writer::context &ctx) const {
    stream << ctx.node_begin()
           << "call_node: { " << full_apeal_name() << " }"
           << ctx.node_end()
           << ctx.edge();

    for(const auto &arg : m_args) {
        const auto child_ctx = ctx.new_child("arg");
        if(arg) {
            arg->write(stream, child_ctx);
        } else {
            stream << child_ctx.node_begin()
                   << "[null arg]"
                   << child_ctx.node_end()
                   << child_ctx.edge();
        }
    }
    return stream;
}

km2::ast_token_type km2::call_node::rvalue_type() const {
    return AstVariable; /// because function returns variable
}

km2::markup_string km2::call_node::hover() const {
    using namespace km2::literals;
    if(const auto& d = declaration()) {
        switch (d->rvalue_type()) {
        case AstClass: return "**class** "_md + m_name;
        case AstFunction: return "**function** "_md + m_name;
        case AstMethod: return "**method** "_md + m_name;
        default: break;
        }
    }
    return "**function** "_md + m_name;
}

wall_e::str_list km2::subtract_namespace_stack(const wall_e::str_list &op0, const wall_e::str_list &op1) {
    auto it0 = op0.begin(), it1 = op1.begin();
    for(;
        it0 != op0.end() && it1 != op1.end();
        ++it0, ++it1
        ) {
        if(*it0 != *it1) {
            break;
        }
    }
    return wall_e::str_list(it0, op0.end());
}
