#include "namespace_node.h"
#include "substitution_node.h"


#include <iostream>

#include <src/km2/backend/unit/unit.h>

#include <src/km2/backend/unit/capabilities/namespace_capability.h>
#include "wall_e/src/macro.h"


km2::namespace_node::namespace_node(
        const wall_e::index &index,
        const wall_e::lex::token &keyword_token,
        const std::shared_ptr<block_node>& block_node
        )
    : abstract_value_node(index, { block_node }),
      m_keyword_token(keyword_token),
      m_block_node(block_node),
      m_context({}) {}

km2::abstract_node::factory km2::namespace_node::create(const lvalue::factory& lval_factory) {
    return [lval_factory](const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) -> wall_e::gram::argument {
        if(debug) std::cout << wall_e::type_name<namespace_node>() << "::create: " << args << std::endl;
        if (args.size() > 1) {
                if(const auto& keyword_token = args[0].option<wall_e::lex::token>()) {
                    return std::make_shared<namespace_node>(
                                index,
                                *keyword_token,
                                args[1].value<std::shared_ptr<block_node>>()
                    );
                }
        }
        return nullptr;
    };
}

wall_e::either<
wall_e::error,
km2::backend::value*
> km2::namespace_node::generate_backend_value(const std::shared_ptr<km2::backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
    if(m_block_node) {
        unit->cap<backend::namespace_capability>()->begin_namespace(full_name());

        if(const auto result = m_block_node->generate_backend_value(unit)) {
        } else {
            return result.left();
        }

        return wall_e::right<backend::value*>(unit->cap<backend::namespace_capability>()->end_namespace());
    }
    return wall_e::right<backend::value*>(nullptr);
}

wall_e::list<wall_e::error> km2::namespace_node::errors() const {
    return m_block_node ? m_block_node->errors() : wall_e::list<wall_e::error> {};
}

wall_e::opt<std::string> km2::namespace_node::name() const {
    //if(m_lvalue.lval_kind() == lvalue::Id) {
    //    return m_lvalue.token().text;
    //} else {
        return std::nullopt;
    //}
}

wall_e::str_list km2::namespace_node::full_name() const {
    if(const auto& n = name()) {
        if(const auto ns = nearest_ancestor<namespace_node>()) {
            auto result = ns->full_name();
            result.push_back(*n);
            return result;
        }
        return wall_e::str_list { *n };
    }
    return wall_e::str_list {};
}

const km2::backend::context &km2::namespace_node::context() const {
    return m_context;
}

std::ostream &km2::namespace_node::short_print(std::ostream &stream) const {
    return stream << "namespace_node: ";//{ name: " << m_lvalue << " }";
}

km2::ast_token_list km2::namespace_node::tokens() const {
    return ast_token_list {
        //ast_token {
        //    .type = m_lvalue.token_type(AstNamespace),
        //    .modifier = m_lvalue.token_modifier(),
        //    .node_type = wall_e::type_name<namespace_node>(),
        //    .hover = hover(),
        //    .text = m_lvalue.token().text,
        //    .segment = m_lvalue.token().segment()
        //},
        ast_token {
            .type = AstKeyword,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<namespace_node>(),
            .hover = hover(),
            .text = m_keyword_token.text,
            .segment = m_keyword_token.segment()
        },
    } + (m_block_node ? m_block_node->tokens() : ast_token_list {});
}


std::ostream &km2::namespace_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    if(fmt == Simple) {
        stream << std::string(ctx.level(), ' ') << "{namespace_node}:" << std::endl;
        //stream << std::string(ctx.level() + 1, ' ') << "name: " << m_name << std::endl;
        if(m_block_node) {
            m_block_node->write(stream, fmt, ctx.new_child("block"));
        } else {
            stream << std::string(ctx.level() + 1, ' ') + "block node not exist" << std::endl;
        }
    } else if(fmt == TreeWriter) {
        const auto fn = full_name();
        stream << ctx.node_begin()
               << "namespace_node: { lvalue: " << lval().map_member_func<std::string>(&lvalue::pretty_str);
        if(fn.size() > 0) stream << ", name: " << fn;
        stream << (m_block_node ? "" : ", no block")
               << " }"
               << ctx.node_end()
               << ctx.edge();

        if(m_block_node) {
            m_block_node->write(stream, fmt, ctx.new_child("block"));
        }
    }
    return stream;
}


km2::ast_token_type km2::namespace_node::rvalue_type() const {
    return AstNamespace;
}

km2::markup_string km2::namespace_node::hover() const {
    using namespace km2::literals;
    if(const auto& lvalue = lval()) {
        switch (lvalue->lval_kind()) {
        case lvalue::Exp: return "**export namespace**"_md;
        case lvalue::Id: return "**namespace** "_md + lvalue->token().text;
        case lvalue::AnonId: return "**anonimus namespace**"_md;
        case lvalue::__kind_max_value: return "**unknown kind namespace**"_md;
        }
    }
    return "**namespace**"_md;
}
