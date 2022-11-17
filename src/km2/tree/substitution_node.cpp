#include "substitution_node.h"
#include <iostream>

std::shared_ptr<const km2::substitution_node> km2::substitution_node::eval_prev_exp_node() const {
    return find_until<substitution_node>([](const std::shared_ptr<const substitution_node>& s) -> bool {
        return s->lvalue().lval_kind() == lvalue::Exp;
    });
}

km2::substitution_node::substitution_node(
        const wall_e::index& index,
        const struct lvalue& lvalue,
        const std::shared_ptr<abstract_value_node>& rvalue
        )
    : abstract_value_node(index, { rvalue }, rvalue ? lvalue.token().segment() + rvalue->segment() : lvalue.token().segment()),
      m_lvalue(lvalue),
      m_rvalue(rvalue) {}

km2::abstract_node::factory km2::substitution_node::create(const lvalue::factory &lval_factory) {
    return [lval_factory](const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) -> wall_e::gram::argument {
        if(debug) std::cout << wall_e::type_name<substitution_node>() << "::create: " << args << std::endl;
        if(args.size() > 2) {
            if(const auto& lval = args[0].option<wall_e::lex::token>().flat_map(lval_factory)) {
                return std::make_shared<substitution_node>(
                        index,
                        *lval,
                        args[2].cast_or<std::shared_ptr<abstract_value_node>>()
                        );
            }
        }
        return nullptr;
    };
}

std::ostream &km2::substitution_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    stream << ctx.node_begin()
           << "substitution_node: { lvalue: " << m_lvalue.pretty_str() << " }"
           << ctx.node_end()
           << ctx.edge();

    if(m_rvalue) m_rvalue->write(stream, fmt, ctx.new_child("rvalue"));
    return stream;
}

std::ostream &km2::substitution_node::short_print(std::ostream &stream) const {
    return stream << "substitution_node: { lvalue: " << m_lvalue.pretty_str() << " }";
}

km2::ast_token_list km2::substitution_node::tokens() const {
    using namespace km2::literals;
    return ast_token_list {
        km2::ast_token {
            .type = m_lvalue.token_type(m_rvalue ? m_rvalue->rvalue_type() : wall_e::enums::null),
            .modifier = m_lvalue.token_modifier(),
            .node_type = wall_e::type_name<substitution_node>(),
            .hover = m_rvalue ? m_rvalue->hover() : ""_md,
            .text = m_lvalue.token().text,
            .segment = m_lvalue.token().segment()
        },
    } + (m_rvalue ? m_rvalue->tokens() : ast_token_list {});
}

wall_e::list<wall_e::error> km2::substitution_node::errors() const {
    const auto rval_errors = m_rvalue ? m_rvalue->errors() : wall_e::list<wall_e::error> {};

    if(m_lvalue.lval_kind() == lvalue::Exp) {
        const auto& p = prev_exp_node();
        if(p) {
            std::cout << "prev_exp_node: " << p << std::endl;
        } else {
            std::cout << "prev_exp_node: null" << std::endl;
        }

        if(p) {
            return wall_e::list<wall_e::error> {
                wall_e::error(
                            "Module can have only one export.",
                            wall_e::error::Err,
                            wall_e::error::Semantic,
                            DoubleExport,
                            m_lvalue.token().segment()
                            )
            } + rval_errors;
        }
    }
    return rval_errors;
}

wall_e::either<wall_e::error, km2::backend::value *> km2::substitution_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    if(m_rvalue) {
        return m_rvalue->generate_backend_value(unit);
    } else {
        return wall_e::left(wall_e::error("rvalue missing"));
    }
}

bool km2::substitution_node::is_export_root() const {
    return m_lvalue.lval_kind() == lvalue::Exp;
}

km2::ast_token_type km2::substitution_node::rvalue_type() const {
    return m_rvalue ? m_rvalue->rvalue_type() : wall_e::enums::null;
}

km2::markup_string km2::substitution_node::hover() const {
    return m_rvalue ? m_rvalue->hover() : km2::markup_string();
}
