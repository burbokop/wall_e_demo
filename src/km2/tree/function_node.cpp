#include "decl_arg_node.h"
#include "function_node.h"
#include "namespace_node.h"


#include <wall_e/src/utility/defer.h>
#include <wall_e/src/utility/token_tools.h>
#include <iostream>
#include <src/km2/backend/unit/unit.h>
#include <src/km2/backend/unit/capabilities/type_capability.h>
#include <src/km2/backend/models/function_ref.h>
#include "wall_e/src/macro.h"


km2::function_node::function_node(
        const wall_e::index &index,
        const wall_e::vec<std::shared_ptr<decl_arg_node>> &args,
        std::shared_ptr<block_node> body
        )
    : abstract_value_node(index, cast_to_children(args, wall_e::vec<std::shared_ptr<abstract_value_node>> { body })),
      m_args(args),
      m_body(body) {}

km2::abstract_node::factory km2::function_node::create() {
    return [](const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) -> wall_e::gram::argument {
        if(debug) std::cout << "km2::function_node::create: " << args << std::endl;
        if(args.size() > 2) {
            wall_e::vec<std::shared_ptr<decl_arg_node>> da_nodes;
            const auto decl_args = args[1].constrain();
            for(const auto& decl_arg : decl_args) {
                const auto da_node = decl_arg.option_cast<std::shared_ptr<km2::decl_arg_node>>();
                if(da_node.has_value()) {
                    da_nodes.push_back(da_node.value());
                }
            }

            return std::make_shared<function_node>(
                        index,
                        da_nodes,
                        args[2].cast_or<std::shared_ptr<block_node>>()
                        );
        }

        return nullptr;
    };
}

wall_e::str_list km2::function_node::full_name() const {
    if(lval()) {
        if(const auto ns = nearest_ancestor<namespace_node>()) {
            return ns->full_name() + lval()->pretty_str();
        }
        return { lval()->pretty_str() };
    }
    return {};
}

wall_e::either<
wall_e::error,
km2::backend::value *
> km2::function_node::generate_backend_value(const std::shared_ptr<km2::backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
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

    if(const auto& name = lval().map<std::string>([](const lvalue& v) { return v.token().text; })) {

        const auto proto = unit->proto(*name, arg_types, unit->cap<backend::type_capability>()->void_type());

        const auto block = unit->begin_block(*name + "_block", proto, arg_names);
        wall_e::defer end_block_defer([unit, block]{
            if(block) {
                unit->create_void_return();
                unit->end_block();
            }
        });

        if(const auto block_node = nearest_ancestor<km2::block_node>()) {
            if(const auto& error = const_cast<km2::block_node*>(block_node)->add_function(backend::function_ref(namepace_name, *name, proto))) {
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
    } else {
        return wall_e::left(wall_e::error("function does not have lvalue"));
    }
}

wall_e::list<wall_e::error> km2::function_node::errors() const {
    return m_args
            .map<wall_e::list<wall_e::error>>([](const std::shared_ptr<decl_arg_node>& arg){ return arg ? arg->errors() : wall_e::list<wall_e::error> {}; })
            .reduce({}, [](const auto& a, const auto& b) { return a.with(b); })
            .with(m_body ? m_body->errors() : wall_e::list<wall_e::error> {});
}

std::ostream &km2::function_node::short_print(std::ostream &stream) const {
    return stream << "function_node { lvalue: " << lval().map_member_func<std::string>(&lvalue::pretty_str) << ", args: " << m_args << " }";
}

km2::ast_token_list km2::function_node::tokens() const {    
    return tokens_from_node_list(m_args) + (m_body ? m_body->tokens() : ast_token_list {});
}

std::ostream &km2::function_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    std::string full_name;
    if(lval()) {
        if(const auto ns = nearest_ancestor<namespace_node>()) {
            full_name = ns->full_name().join("::") + "::" + lval()->pretty_str();
        } else {
            full_name = lval()->pretty_str();
        }
    }

    if(fmt == Simple) {
        stream << std::string(ctx.level(), ' ') << "{function_node}:" << std::endl;
        stream << std::string(ctx.level() + 1, ' ') << "name: " << full_name << std::endl;
        stream << std::string(ctx.level() + 1, ' ') << "args: " << std::endl;

        for(const auto& a : m_args) {
            if(a) {
                a->write(stream, fmt, ctx.new_child("arg"));
            } else {
                stream << std::string(ctx.level() + 1, ' ') + "null arg" << std::endl;
            }
        }
        stream << std::string(ctx.level() + 1, ' ') << "body: " << std::endl;
        if(m_body) {
            m_body->write(stream, fmt, ctx.new_child("body"));
        } else {
            stream << std::string(ctx.level() + 1, ' ') + "body not exist" << std::endl;
        }
    } else if(fmt == TreeWriter) {
        stream << ctx.node_begin()
               << "function_node { name: " << full_name << (m_body ? "" : ", no body") << " }"
               << ctx.node_end()
               << ctx.edge();

        for(const auto& a : m_args) {
            const auto child_ctx = ctx.new_child("arg");
            if(a) {
                a->write(stream, fmt, child_ctx);
            } else {
                stream << child_ctx.node_begin()
                       << "[null arg]"
                       << child_ctx.node_end()
                       << child_ctx.edge();
            }
        }
        if(m_body) {
            m_body->write(stream, fmt, ctx.new_child("body"));
        }
    }
    return stream;
}


km2::ast_token_type km2::function_node::rvalue_type() const {
    return AstFunction;
}

km2::markup_string km2::function_node::hover() const {
    using namespace km2::literals;
    if(const auto& lvalue = lval()) {
        switch (lvalue->lval_kind()) {
        case lvalue::Exp: return "**export function**"_md;
        case lvalue::Id: return "**function** "_md + km2::markup_string(full_name().join("::"), km2::markup_string::PlainText);
        case lvalue::AnonId: return "**anonimus function**"_md;
        case lvalue::__kind_max_value: break;
        }
    }
    return "**unknown kind function**"_md;
}
