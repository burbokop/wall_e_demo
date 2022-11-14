#include "asm_node.h"
#include "wall_e/src/macro.h"

#include <iostream>

km2::asm_node::asm_node(const wall_e::index &index, const std::string& text, const wall_e::text_segment &segment)
    : km2::abstract_value_node(index, {}, wall_e::text_segment()),
      m_text(text) {}

wall_e::gram::argument km2::asm_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index, const wall_e::gram::environment* env) {
    if(args.size() > 2 && args[2].contains_type<wall_e::lex::token>()) {
        const auto token = args[2].value<wall_e::lex::token>();
        if(token.text.size() > 2) {
            auto text = token.text;
            text.erase(text.begin(), text.begin() + 1);
            text.erase(text.end() - 1, text.end());
            text = wall_e::lex::trim(text, '\t');
            text = wall_e::lex::trim(text);
            return std::make_shared<asm_node>(index, text, token.segment());
        }
        return {};
    }
    return wall_e::gram::pattern::default_processor(args, index, env);
}

wall_e::either<
    wall_e::error,
    km2::backend::value*
> km2::asm_node::generate_backend_value(const std::shared_ptr<backend::unit> &unit) {
    if(debug) std::cout << wall_e_this_function << std::endl;
    return wall_e::left(wall_e::error("asm_node not implemented"));
}

std::ostream &km2::asm_node::write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const {
    return short_print(stream);
}

wall_e::list<wall_e::error> km2::asm_node::errors() const {
    return { wall_e::error("errors not implemented in " + wall_e::type_name<asm_node>()) };
}

std::ostream &km2::asm_node::short_print(std::ostream &stream) const {
    return stream << "asm_node { not implemented }";
}

wall_e::list<km2::ast_token> km2::asm_node::tokens() const {
    using namespace km2::literals;
    return {
        ast_token {
            .type = AstKeyword,
            .modifier = wall_e::enums::max_value<ast_token_modifier>(),
            .node_type = wall_e::type_name<asm_node>(),
            .hover = "**assembly code** ( not implemented )"_md,
            .text = m_text,
            .segment = this->segment()
        }
    };
}
