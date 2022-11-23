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

#if __has_include(<wall_e/src/gram.h>)
    #include <wall_e/src/gram.h>
#else
    #include <wall_e/gram.h>
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
    std::string m_uri;
    wall_e::lex::token_vec m_tokens;
    wall_e::lex::token_vec m_comments;
    std::string m_rules;
    wall_e::variant m_token_tree;
    std::shared_ptr<km2::abstract_node> m_root_node;
    std::shared_ptr<backend::unit> m_unit;
    backend::value* m_backend_value;

    wall_e::list<wall_e::error> m_errors;
    std::shared_ptr<wall_e::gram::log> m_gram_log;

    mutable wall_e::opt<km2::ast_token_list> m_ast_tokens;
    mutable wall_e::opt<wall_e::map<wall_e::text_segment, markup_string>> m_hovers;
public:
    compilation_result(
            const std::string& uri,
            const wall_e::lex::token_vec& tokens,
            const wall_e::lex::token_vec& comments,
            const std::string& rules,
            const wall_e::variant& token_tree,
            const std::shared_ptr<km2::abstract_node>& root_node,
            const std::shared_ptr<backend::unit>& unit,
            backend::value*const backend_value,
            const wall_e::list<wall_e::error>& errors,
            const std::shared_ptr<wall_e::gram::log>& gram_log
            )
        : m_uri(uri),
          m_tokens(tokens),
          m_comments(comments),
          m_rules(rules),
          m_token_tree(token_tree),
          m_root_node(root_node),
          m_unit(unit),
          m_backend_value(backend_value),
          m_errors(errors),
          m_gram_log(gram_log) {}

    const std::string& uri() const { return m_uri; }

    const wall_e::lex::token_vec& tokens() const { return m_tokens; }
    const wall_e::lex::token_vec& comments() const { return m_comments; };

    const std::string& rules() const { return m_rules; }
    const wall_e::variant& token_tree() const { return m_token_tree; }
    const std::shared_ptr<km2::abstract_node>& root_node() const { return m_root_node; }
    const std::shared_ptr<backend::unit>& unit() const { return m_unit; }
    backend::value*const backend_value() const { return m_backend_value; }

    const wall_e::list<wall_e::error>& errors() const { return m_errors; }

    const km2::ast_token_list& ast_tokens() const;
    const wall_e::map<wall_e::text_segment, markup_string>& hovers() const;
    const std::shared_ptr<wall_e::gram::log>& gram_log() const { return m_gram_log; }

};

std::ostream& operator<<(std::ostream& stream, const compilation_result& res);

enum flag {
    OnlyTree = 1,
    Verbose  = 2
    ///      = 4
};

typedef wall_e::list<flag> flags;

class environment : public wall_e::gram::environment {
    const flags& m_flags;
public:
    environment(const std::string& uri, const flags& flags)
        : wall_e::gram::environment(uri),
          m_flags(flags) {}

    const flags& flags() const { return m_flags; }
    virtual bool verbose() const override { return m_flags.contains(Verbose); }

};

std::ostream& default_log();

compilation_result compile(const backend::backend* b, const std::string &input, const std::string& uri, const flags &flags = {}, std::ostream& log = default_log());
compilation_result compile(const backend::backend* b, std::istream &input, const std::string& uri, const flags &flags = {}, std::ostream& log = default_log());
inline compilation_result compile(const backend::backend* b, std::istream &&input, const std::string& uri, const flags &flags = {}, std::ostream& log = default_log()) {
    std::istream& i = input;
    return compile(b, i, uri, flags, log);
}
//inline compilation_result compile(const std::shared_ptr<const backend::backend>& b, const std::string &input, const flags &flags = {}) {
//    const std::shared_ptr<const backend::backend> b_copy = b;
//    return compile(b_copy.get(), input, flags);
//}

}

#endif // TEST2_H
