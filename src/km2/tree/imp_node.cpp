#include "imp_node.h"

#include <iostream>

km2::imp_node::imp_node(
        const wall_e::index &index,
        const wall_e::lex::token &keyword_token,
        const wall_e::lex::token &name_token
        ) : abstract_value_node(index, {}, keyword_token.segment() + name_token.segment()),
            m_keyword_token(keyword_token),
            m_name_token(name_token) {}

wall_e::gram::argument km2::imp_node::create(const wall_e::gram::arg_vector &args, const wall_e::index &index) {
    if(debug) std::cout << "km2::imp_node::create: " << args << std::endl;
    if(args.size() > 1) {
        return std::make_shared<imp_node>(
                    index,
                    args[0].value_or<wall_e::lex::token>(),
                    args[1].value_or<wall_e::lex::token>()
                );
    }
    return std::make_shared<imp_node>(index, wall_e::lex::token(), wall_e::lex::token());
}


std::ostream &km2::imp_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    return short_print(stream << ctx.node_begin()) << ctx.node_end() << ctx.edge();
}

std::ostream &km2::imp_node::short_print(std::ostream &stream) const {
    return stream << "imp_node { name: " << m_name_token.text << " }";
}

km2::ast_token_list km2::imp_node::tokens() const {
    using namespace km2::literals;
    const auto hover = "**module** "_md + m_name_token.text;
    return wall_e::list<km2::ast_token> {
        ast_token {
            .type = AstKeyword,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<imp_node>(),
            .hover = hover,
            .text = m_keyword_token.text,
            .segment = m_keyword_token.segment()
        },
        ast_token {
            .type = AstMacro /** TODO use type depending on what exported (namespace, variable, function, ...) */,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<imp_node>(),
            .hover = hover,
            .text = m_name_token.text,
            .segment = m_name_token.segment()
        }
    };
}

wall_e::list<wall_e::error> km2::imp_node::errors() const { return {}; }

wall_e::either<wall_e::error, km2::backend::value *> km2::imp_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    return wall_e::left(wall_e::error("not implemented", wall_e::error::err, wall_e::error::semantic, 0, segment()));
}
