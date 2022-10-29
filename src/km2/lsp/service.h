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

#if __has_include(<src/km2/tree/abstract/abstract_node.h>)
    #include <src/km2/tree/abstract/abstract_node.h>
#else
    #include <km2/tree/abstract/abstract_node.h>
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

std::vector<semantic_token_type> parse_semantic_token_types(const std::vector<std::string>& toks);
std::vector<semantic_token_modifier> parse_semantic_token_modifiers(const std::vector<std::string>& modys);

std::vector<std::string> stringify_semantic_token_types(const std::vector<semantic_token_type>& toks);
std::vector<std::string> stringify_semantic_token_modifiers(const std::vector<semantic_token_modifier>& modys);

struct text_position {
    std::size_t line;
    std::size_t character;
};

struct text_range {
    text_position begin;
    text_position end;
    static text_range from_text_segment(const wall_e::text_segment& segment, const std::string& text);
};

struct text_wide_position {
    std::size_t line;
    std::size_t character;
    std::size_t length;
    static text_wide_position from_text_segment(const wall_e::text_segment& segment, const std::string& text);
};

struct semantic_token {
    semantic_token_type type;
    semantic_token_modifier modifier;
    wall_e::text_segment segment;
    text_wide_position position;
};

struct semantic_tokens_client_capability {
    std::vector<semantic_token_type> token_types;
    std::vector<semantic_token_modifier> token_modifiers;
};

struct semantic_tokens_legend {
    std::vector<semantic_token_type> token_types;
    std::vector<semantic_token_modifier> token_modifiers;
};

const std::map<std::string, semantic_token_type>& default_semantic_token_types_map();
const std::map<km2::ast_token_type, semantic_token_type>& default_ast_semantic_token_types_map();

class service {
    struct cache_item {
        std::string content;
        km2::flags flags;
        km2::compilation_result compilation_result;
    };
    typedef std::string uri_t;

    std::map<uri_t, cache_item> m_cache;
    std::map<std::string, semantic_token_type> m_semantic_token_types_map;
    std::map<km2::ast_token_type, semantic_token_type> m_ast_semantic_token_types_map;
    //std::map<std::string, semantic_token_modifier> m_semantic_token_modifiers_map; TODO
public:
    service();

    void configure(const std::string& uri, const km2::flags& flags);

    semantic_tokens_legend register_semantic_tokens(const semantic_tokens_client_capability& client_cap);

    void initialize(const std::string& uri);
    std::list<wall_e::error> change_content(const std::string& uri, const std::string& content);
    std::vector<wall_e::lex::token> tokens(const std::string& uri) const;
    std::vector<semantic_token> semantic_tokens(const std::string& uri) const;
    std::optional<std::string> hover(const std::string& uri, const wall_e::text_segment::predicate& predicate);
    std::list<std::string> complete(const std::string& uri, const wall_e::text_segment::predicate& predicate);
};

}
}

#endif // KM2LSP_SERVICE_H


