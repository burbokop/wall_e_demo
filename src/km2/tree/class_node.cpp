#include "class_node.h"

#include "block_node.h"
#include "decl_arg_node.h"
#include <iostream>
#include <src/km2/tree/traits/callable_trait.h>

km2::class_node::class_node(
        const wall_e::gram::environment *env,
        const wall_e::index &index,
        const wall_e::lex::token &keyword_token,
        const wall_e::vec<std::shared_ptr<decl_arg_node>> &args,
        std::shared_ptr<block_node> body
        )
    : abstract_type_node(
          env,
          index,
          cast_to_children(args, wall_e::vec<std::shared_ptr<abstract_value_node>> { body }),
          {},
          wall_e::box_list<node_trait>::factory::make<callable_trait>()
          ),
      m_keyword_token(keyword_token),
      m_args(args),
      m_body(body) {}

km2::abstract_node::factory km2::class_node::create() {
    return [](const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) -> wall_e::gram::argument {
        if(env->verbose()) std::cout << "km2::class_node::create: " << args << std::endl;
        if(args.size() > 3) {
            wall_e::vec<std::shared_ptr<decl_arg_node>> da_nodes;
            const auto decl_args = args[2].constrain();
            for(const auto& decl_arg : decl_args) {
                const auto da_node = decl_arg.option_cast<std::shared_ptr<km2::decl_arg_node>>();
                if(da_node.has_value()) {
                    da_nodes.push_back(da_node.value());
                }
            }

            return std::make_shared<class_node>(
                env,
                index,
                args[0].value_or<wall_e::lex::token>(),
                da_nodes,
                args[3].cast_or<std::shared_ptr<block_node>>()
            );
        }

        return nullptr;
    };
}


std::ostream &km2::class_node::write(std::ostream &stream, const wall_e::tree_writer::context &ctx) const {
    stream << ctx.node_begin()
           << "function_node { name: " << lval_full_name() << (m_body ? "" : ", no body") << " }"
           << ctx.node_end()
           << ctx.edge();

    for(const auto& a : m_args) {
        const auto child_ctx = ctx.new_child("arg");
        if(a) {
            a->write(stream, child_ctx);
        } else {
            stream << child_ctx.node_begin()
                   << "[null arg]"
                   << child_ctx.node_end()
                   << child_ctx.edge();
        }
    }
    if(m_body) {
        m_body->write(stream, ctx.new_child("body"));
    }
    return stream;
}

std::ostream &km2::class_node::short_print(std::ostream &stream) const {
    return stream << "class { " << lval_full_name() << " }";
}

km2::ast_token_list km2::class_node::tokens() const {
    return km2::ast_token_list {
        km2::ast_token {
            .type = AstKeyword,
            .modifier = wall_e::enums::null,
            .node_type = wall_e::type_name<class_node>(),
            .hover = hover(),
            .text = m_keyword_token.text,
            .segment = m_keyword_token.segment()
        }
    } + tokens_from_node_list(m_args) + (m_body ? m_body->tokens() : ast_token_list {});
}

km2::ast_token_type km2::class_node::rvalue_type() const {
    return AstClass;
}

km2::markup_string km2::class_node::hover() const {
    using namespace km2::literals;
    if(const auto& lvalue = lval()) {
        switch (lvalue->lval_kind()) {
        case lvalue::Exp: return "**export class**"_md;
        case lvalue::Id: return "**class** "_md + km2::markup_string(lval_full_name().join("::"), km2::markup_string::PlainText);
        case lvalue::AnonId: return "**anonimus class**"_md;
        case lvalue::__kind_max_value: break;
        }
    }
    return "**unknown kind class**"_md;
}

wall_e::list<wall_e::error> km2::class_node::errors() const {
    return m_args
            .map<wall_e::list<wall_e::error>>([](const std::shared_ptr<decl_arg_node>& arg){ return arg ? arg->errors() : wall_e::list<wall_e::error> {}; })
            .reduce({}, [](const auto& a, const auto& b) { return a.with(b); })
            .with(m_body ? m_body->errors() : wall_e::list<wall_e::error> {});
}

wall_e::either<wall_e::error, km2::backend::value *> km2::class_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    return wall_e::right<backend::value*>(nullptr);
}

wall_e::either<wall_e::error, km2::backend::type *> km2::class_node::generate_backend_type(const std::shared_ptr<backend::unit> &unit) {
    return wall_e::right<backend::type*>(nullptr);
}
