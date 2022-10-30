#include "asm_node.h"

#include <iostream>

km2::asm_node::asm_node(const wall_e::index &index, const std::string& text, const wall_e::text_segment &segment)
    : km2::abstract_value_node(index, {}, wall_e::text_segment()),
      m_text(text) {}

wall_e::gram::argument km2::asm_node::create(const wall_e::gram::arg_vector &args, const wall_e::index& index) {
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
    return wall_e::gram::pattern::default_processor(args, index);
}

wall_e::either<
    wall_e::error,
    llvm::Value *
> km2::asm_node::generate_llvm(const std::shared_ptr<translation_unit> &unit) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return wall_e::left(wall_e::error("asm_node not implemented"));
}

void km2::asm_node::print(size_t level, std::ostream &stream) const {
    short_print(stream);
}

wall_e::list<wall_e::error> km2::asm_node::errors() const {
    return {};
}

void km2::asm_node::short_print(std::ostream &stream) const {
    stream << "asm_node { not implemented }";
}

wall_e::list<km2::ast_token> km2::asm_node::tokens() const {
    return {
        ast_token {
            .type = AstKeyword,
            .node_type = wall_e::type_name<asm_node>(),
            .hover = "<b>assembly code</b> ( not implemented )",
            .text = m_text,
            .segment = this->segment()
        }
    };
}
