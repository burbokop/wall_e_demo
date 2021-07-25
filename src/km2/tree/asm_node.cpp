#include "asm_node.h"


km2::asm_node::asm_node(const std::string& text) {

}

wall_e::gram::argument km2::asm_node::create(const wall_e::gram::arg_vector &args) {
    if(args.size() > 2 && args[2].contains_type<wall_e::lex::token>()) {
        const auto token = args[2].value<wall_e::lex::token>();
        if(token.text.size() > 2) {
            auto text = token.text;
            text.erase(text.begin(), text.begin() + 1);
            text.erase(text.end() - 1, text.end());
            text = wall_e::lex::trim(text, '\t');
            text = wall_e::lex::trim(text);
            return new asm_node(text);
        }
        return {};
    }
    return wall_e::gram::pattern::default_processor(args);
}

llvm::Value *km2::asm_node::generate_llvm(module_builder *builder) {

}

void km2::asm_node::print(size_t level, std::ostream &stream) {

}
