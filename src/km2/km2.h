#ifndef KM2_H
#define KM2_H

#include "error.h"

#include <string>


#if __has_include(<wall_e/src/models/variant.h>)
    #include <wall_e/src/models/variant.h>
#else
    #include <wall_e/models/variant.h>
#endif

#if __has_include(<wall_e/src/lex.h>)
    #include <wall_e/src/lex.h>
#else
    #include <wall_e/lex.h>
#endif

#if __has_include(<src/km2/ast_token.h>)
    #include <src/km2/ast_token.h>
#else
    #include <km2/ast_token.h>
#endif

namespace llvm {
class Value;
}

namespace km2 {

class translation_unit;
class abstract_node;

class compilation_result {
    wall_e::lex::token_vec m_tokens;
    std::string m_rules;
    wall_e::variant m_token_tree;
    std::shared_ptr<km2::abstract_node> m_root_node;
    std::shared_ptr<translation_unit> m_unit;
    llvm::Value* m_llvm_value;

    std::list<wall_e::error> m_errors;

    mutable wall_e::opt<km2::ast_token_list> m_ast_tokens;
    mutable wall_e::opt<std::map<wall_e::text_segment, std::string>> m_hovers;
public:
    compilation_result(
            const wall_e::lex::token_vec& tokens,
            const std::string& rules,
            const wall_e::variant& token_tree,
            const std::shared_ptr<km2::abstract_node>& root_node,
            const std::shared_ptr<translation_unit>& unit,
            llvm::Value*const llvm_value,
            const std::list<wall_e::error>& errors
            )
        : m_tokens(tokens),
          m_rules(rules),
          m_token_tree(token_tree),
          m_root_node(root_node),
          m_unit(unit),
          m_llvm_value(llvm_value),
          m_errors(errors) {}

    const wall_e::lex::token_vec& tokens() const { return m_tokens; }
    const std::string& rules() const { return m_rules; }
    const wall_e::variant& token_tree() const { return m_token_tree; }
    const std::shared_ptr<km2::abstract_node>& root_node() const { return m_root_node; }
    const std::shared_ptr<translation_unit>& unit() const { return m_unit; }
    llvm::Value*const llvm_value() const { return m_llvm_value; }

    const std::list<wall_e::error>& errors() const { return m_errors; }

    const km2::ast_token_list& ast_tokens() const;
    const std::map<wall_e::text_segment, std::string>& hovers() const;
};

enum flag {
    only_tree,
    verbose,
    generate_lsp_info
};

typedef std::list<flag> flags;

std::list<std::string> key_names();

compilation_result compile(const std::string &input, const flags &flags = {});

}

#endif // TEST2_H
