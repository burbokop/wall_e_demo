#ifndef KM2LSP_SERVICE_H
#define KM2LSP_SERVICE_H

#include <optional>
#include <string>
#include <map>
#include <list>

#if __has_include(<src/km2/km2.h>)
    #include <src/km2/km2.h>
#else
    #include <km2/km2.h>
#endif

#if __has_include(<src/km2/ast_token.h>)
    #include <src/km2/ast_token.h>
#else
    #include <km2/ast_token.h>
#endif

#if __has_include(<wall_e/src/enums.h>)
    #include <wall_e/src/enums.h>
#else
    #include <wall_e/enums.h>
#endif

namespace km2 {

namespace lsp {


/** Language Server Protocol Specification - 3.17 */
wall_e_enum(semantic_token_type,
    Namespace = 0,
    Type,
    Class,
    Enum,
    Interface,
    Struct,
    TypeParameter,
    Parameter,
    Variable,
    Property,
    EnumMember,
    Event,
    Function,
    Method,
    Macro,
    Keyword,
    Modifier,
    Comment,
    String,
    Number,
    Regexp,
    Operator,
    Decorator
)

/** Language Server Protocol Specification - 3.17 */
wall_e_enum(semantic_token_modifier,
    Declaration = 0,
    Definition,
    Readonly,
    Static,
    Deprecated,
    Abstract,
    Async,
    Modification,
    Documentation,
    DefaultLibrary
)

std::string first_char_to_lower(std::string str);
std::string first_char_to_upper(std::string str);

wall_e::vec<semantic_token_type> parse_semantic_token_types(const wall_e::vec<std::string>& toks);
wall_e::vec<semantic_token_modifier> parse_semantic_token_modifiers(const wall_e::vec<std::string>& modys);

wall_e::vec<std::string> stringify_semantic_token_types(const wall_e::vec<semantic_token_type>& toks);
wall_e::vec<std::string> stringify_semantic_token_modifiers(const wall_e::vec<semantic_token_modifier>& modys);

struct semantic_token {
    semantic_token_type type;
    semantic_token_modifier modifier;
    wall_e::text_segment segment;
    inline bool operator ==(const semantic_token& other) const {
        return type == other.type && modifier == other.modifier && segment == other.segment;
    }
    friend inline std::ostream& operator<<(std::ostream& stream, const semantic_token& t) {
        return stream << "{ type: " << t.type
                      << ", modifier: " << t.modifier
                      << ", segment: " << t.segment
                      << " }";
    }
};

struct semantic_tokens_client_capability {
    wall_e::vec<semantic_token_type> token_types;
    wall_e::vec<semantic_token_modifier> token_modifiers;
    inline bool operator ==(const semantic_tokens_client_capability& other) const {
        return token_types == other.token_types && token_modifiers == other.token_modifiers;
    }
};

struct semantic_tokens_legend {
    wall_e::vec<semantic_token_type> token_types;
    wall_e::vec<semantic_token_modifier> token_modifiers;
    inline bool operator ==(const semantic_tokens_legend& other) const {
        return token_types == other.token_types && token_modifiers == other.token_modifiers;
    }
};

const wall_e::map<std::string, semantic_token_type>& default_semantic_token_types_map();
const wall_e::map<km2::ast_token_type, semantic_token_type>& default_ast_semantic_token_types_map();

const wall_e::map<std::string, semantic_token_modifier>& default_semantic_token_modifiers_map();
const wall_e::map<km2::ast_token_modifier, semantic_token_modifier>& default_ast_semantic_token_modifiers_map();

wall_e::vec<semantic_token> split_multiline_sementic_tokens(
        const wall_e::vec<semantic_token>& tokens,
        const wall_e::list<wall_e::text_segment>& lines
        );

class service {
    struct cache_item {
        std::string content;
        wall_e::list<wall_e::text_segment> lines;
        wall_e::opt<km2::compilation_result> compilation_result;
    };
    typedef std::string uri_t;
    km2::flags m_flags;
    std::ostream& m_log;

    wall_e::map<uri_t, cache_item> m_cache;
    wall_e::map<std::string, semantic_token_type> m_semantic_token_types_map;
    wall_e::map<km2::ast_token_type, semantic_token_type> m_ast_semantic_token_types_map;

    wall_e::map<std::string, semantic_token_modifier> m_semantic_token_modifiers_map;
    wall_e::map<km2::ast_token_modifier, semantic_token_modifier> m_ast_semantic_token_modifiers_map;
public:
    service(const km2::flags& flags = {}, std::ostream& log = default_log())
        : m_flags(flags),
          m_log(log) {}

    semantic_tokens_legend register_semantic_tokens(const semantic_tokens_client_capability& client_cap);

    bool ast_tokens_ready(const std::string& uri) const;

    wall_e::list<wall_e::error> change_content(const std::string& uri, const std::string& content);

    wall_e::vec<wall_e::lex::token> tokens(const std::string& uri) const;
    wall_e::vec<semantic_token> semantic_tokens(const std::string& uri, bool split_multiline) const;
    wall_e::opt<markup_string> hover(const std::string& uri, const wall_e::text_segment::predicate& predicate) const;
    inline wall_e::opt<markup_string> hover(const std::string& uri, std::size_t offset) const {
        return hover(uri, wall_e::text_segment::offset_predicate(offset));
    }
    wall_e::list<std::string> complete(const std::string& uri, const wall_e::text_segment::predicate& predicate);
};

}
}

#endif // KM2LSP_SERVICE_H


