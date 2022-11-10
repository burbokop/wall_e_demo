#ifndef KM2_H
#define KM2_H

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


namespace km2 {

namespace backend {
class unit;
class value;
class backend;
}
class abstract_node;

class compilation_result {
    wall_e::lex::token_vec m_tokens;
    std::string m_rules;
    wall_e::variant m_token_tree;
    std::shared_ptr<km2::abstract_node> m_root_node;
    std::shared_ptr<backend::unit> m_unit;
    backend::value* m_backend_value;

    wall_e::list<wall_e::error> m_errors;

    mutable wall_e::opt<km2::ast_token_list> m_ast_tokens;
    mutable wall_e::opt<std::map<wall_e::text_segment, markup_string>> m_hovers;
public:
    compilation_result(
            const wall_e::lex::token_vec& tokens,
            const std::string& rules,
            const wall_e::variant& token_tree,
            const std::shared_ptr<km2::abstract_node>& root_node,
            const std::shared_ptr<backend::unit>& unit,
            backend::value*const backend_value,
            const wall_e::list<wall_e::error>& errors
            )
        : m_tokens(tokens),
          m_rules(rules),
          m_token_tree(token_tree),
          m_root_node(root_node),
          m_unit(unit),
          m_backend_value(backend_value),
          m_errors(errors) {}

    const wall_e::lex::token_vec& tokens() const { return m_tokens; }
    const std::string& rules() const { return m_rules; }
    const wall_e::variant& token_tree() const { return m_token_tree; }
    const std::shared_ptr<km2::abstract_node>& root_node() const { return m_root_node; }
    const std::shared_ptr<backend::unit>& unit() const { return m_unit; }
    backend::value*const backend_value() const { return m_backend_value; }

    const wall_e::list<wall_e::error>& errors() const { return m_errors; }

    const km2::ast_token_list& ast_tokens() const;
    const std::map<wall_e::text_segment, markup_string>& hovers() const;
};

enum flag {
    only_tree,
    verbose
};

typedef std::list<flag> flags;

compilation_result compile(const backend::backend* b, const std::string &input, const flags &flags = {});
compilation_result compile(const backend::backend* b, std::istream &input, const flags &flags = {});
inline compilation_result compile(const backend::backend* b, std::istream &&input, const flags &flags = {}) {
    std::istream& i = input;
    return compile(b, i, flags);
}
//inline compilation_result compile(const std::shared_ptr<const backend::backend>& b, const std::string &input, const flags &flags = {}) {
//    const std::shared_ptr<const backend::backend> b_copy = b;
//    return compile(b_copy.get(), input, flags);
//}

}

#endif // TEST2_H
