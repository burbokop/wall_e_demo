#include "service.h"
#include "../tree/abstract/abstract_node.h"

std::string km2::lsp::first_char_to_lower(std::string str) {
    if(str.size() > 0) {
        str[0] = std::tolower(str[0]);
    }
    return std::move(str);
}

std::string km2::lsp::first_char_to_upper(std::string str) {
    if(str.size() > 0) {
        str[0] = std::toupper(str[0]);
    }
    return std::move(str);
}




km2::lsp::service::service() {

}

void km2::lsp::service::configure(const std::string &uri, const flags &flags) {
    m_cache[uri].flags = flags;
}

km2::lsp::semantic_tokens_legend km2::lsp::service::register_semantic_tokens(const semantic_tokens_client_capability &client_cap) {
    m_semantic_token_types_map.clear();

    const auto& all_token_types = wall_e::enums::reflect_names<semantic_token_type>();

    m_semantic_token_types_map = default_semantic_token_types_map();
    m_ast_semantic_token_types_map = default_ast_semantic_token_types_map();

    std::vector<semantic_token_type> legent_types;
    legent_types.reserve(all_token_types.size());
    for(std::size_t i = 0; i < all_token_types.size(); ++i) {
        const auto type = semantic_token_type(i);
        if(std::find(client_cap.token_types.begin(), client_cap.token_types.end(), type) != client_cap.token_types.end()) {
            legent_types.push_back(type);
        } else {
            legent_types.push_back(wall_e::enums::max_value<semantic_token_type>());
        }
    }

    return semantic_tokens_legend {
        .token_types = legent_types,
        .token_modifiers = {}
    };
}

void km2::lsp::service::initialize(const std::string &uri) {}

std::list<wall_e::error> km2::lsp::service::change_content(const std::string &uri, const std::string &content) {
    auto& ref = m_cache[uri];
    ref.content = content;
    ref.compilation_result = km2::compile(nullptr, content, ref.flags);
    return ref.compilation_result->errors();
}

std::vector<wall_e::lex::token> km2::lsp::service::tokens(const std::string &uri) const {
    const auto cache_it = m_cache.find(uri);
    if(cache_it != m_cache.end() && cache_it->second.compilation_result) {
        return cache_it->second.compilation_result->tokens();
    }
    return {};
}

std::vector<km2::lsp::semantic_token> km2::lsp::service::semantic_tokens(const std::string &uri) const {
    const auto cache_it = m_cache.find(uri);
    if(cache_it != m_cache.end() && cache_it->second.compilation_result) {
        if(cache_it->second.compilation_result->root_node() && m_ast_semantic_token_types_map.size() > 0) {
            const auto& tokens = cache_it->second.compilation_result->ast_tokens();
            if(!tokens.empty()) {
                std::vector<semantic_token> result;
                result.reserve(tokens.size());
                for(const auto& t : tokens) {
                    const auto& ttype_it = m_ast_semantic_token_types_map.find(t.type);
                    if(ttype_it != m_ast_semantic_token_types_map.end()) {
                        result.push_back(semantic_token {
                            .type = ttype_it->second,
                            .modifier = wall_e::enums::max_value<semantic_token_modifier>(),
                            .segment = t.segment,
                            .position = text_wide_position::from_text_segment(t.segment, cache_it->second.content)
                        });
                    }
                }
                return result;
            }
        }

        {
            const auto& tokens = cache_it->second.compilation_result->tokens();
            std::vector<semantic_token> result;
            result.reserve(tokens.size());
            for(const auto& t : tokens) {
                const auto& ttype_it = m_semantic_token_types_map.find(t.name);
                if(ttype_it != m_semantic_token_types_map.end()) {
                    result.push_back(semantic_token {
                        .type = ttype_it->second,
                        .modifier = wall_e::enums::max_value<semantic_token_modifier>(),
                        .segment = t.segment(),
                        .position = text_wide_position::from_text_segment(t.segment(), cache_it->second.content)
                    });
                }
            }
            return result;
        }
    }
    return std::vector<semantic_token> {};
}

std::optional<std::string> km2::lsp::service::hover(const std::string &uri, const wall_e::text_segment::predicate &predicate) {
    const auto cache_it = m_cache.find(uri);
    if(cache_it != m_cache.end() && cache_it->second.compilation_result) {

        const auto& hovers = cache_it->second.compilation_result->hovers();
        const auto it = std::find_if(hovers.begin(), hovers.end(), [&predicate, &cache_it](
                                     const std::pair<wall_e::text_segment, std::string>& pair
                                     ){
            return predicate(pair.first, cache_it->second.content);
        });
        if(it != hovers.end()) {
            return it->second;
        }
    }
    return std::nullopt;
}

std::list<std::string> km2::lsp::service::complete(const std::string &uri, const wall_e::text_segment::predicate &predicate) {
    //const auto cache_it = m_cache.find(uri);
    //if(cache_it != m_cache.end()) {
//
    //    const auto& hovers = cache_it->second.compilation_result.hovers;
    //    const auto it = std::find_if(hovers.begin(), hovers.end(), [&predicate, &cache_it](
    //                                 const std::pair<wall_e::text_segment, std::string>& pair
    //                                 ){
    //        return predicate(pair.first, cache_it->second.content);
    //    });
    //    if(it != hovers.end()) {
    //        return { it->second };
    //    }
    //}
    return key_names();
}




std::vector<km2::lsp::semantic_token_type> km2::lsp::parse_semantic_token_types(const std::vector<std::string> &toks) {
    std::vector<km2::lsp::semantic_token_type> result;
    result.reserve(toks.size());
    for(const auto& t : toks) {
        if(const auto& r = wall_e::enums::from_string<km2::lsp::semantic_token_type>(first_char_to_upper(t))) {
            result.push_back(*r);
        }
    }
    return result;
}

std::vector<km2::lsp::semantic_token_modifier> km2::lsp::parse_semantic_token_modifiers(const std::vector<std::string> &modys) {
    std::vector<km2::lsp::semantic_token_modifier> result;
    result.reserve(modys.size());
    for(const auto& t : modys) {
        if(const auto& r = wall_e::enums::from_string<km2::lsp::semantic_token_modifier>(first_char_to_upper(t))) {
            result.push_back(*r);
        }
    }
    return result;
}

std::vector<std::string> km2::lsp::stringify_semantic_token_types(const std::vector<semantic_token_type> &toks) {
    std::vector<std::string> result;
    result.reserve(toks.size());
    for(const auto& t : toks) {
        result.push_back(first_char_to_lower(wall_e::enums::to_string(t)));
    }
    return result;
}

std::vector<std::string> km2::lsp::stringify_semantic_token_modifiers(const std::vector<semantic_token_modifier> &modys) {
    std::vector<std::string> result;
    result.reserve(modys.size());
    for(const auto& t : modys) {
        result.push_back(first_char_to_lower(wall_e::enums::to_string(t)));
    }
    return result;
}

km2::lsp::text_wide_position km2::lsp::text_wide_position::from_text_segment(const wall_e::text_segment &segment, const std::string &text) {
    text_wide_position result = { .line = 0, .character = 0, .length = 0 };
    std::size_t i = 0;
    std::size_t char_pos = 0;
    bool begin_found = false;
    bool end_found = false;
    for(const auto& c : text) {
        if(i == segment.begin()) {
            result.character = char_pos;
            begin_found = true;
            if(end_found) {
                result.length = 0;
                return result;
            }
        }
        if(i == segment.end()) {
            if(begin_found) {
                result.length = char_pos - result.character;
                return result;
            }
            end_found = true;
        }

        if(c == '\n') {
            if(begin_found) {
                result.length = char_pos - result.character;
                return result;
            }
            ++result.line;
            char_pos = 0;
        } else {
            ++char_pos;
        }
        ++i;
    }
    return result;
}

km2::lsp::text_range km2::lsp::text_range::from_text_segment(const wall_e::text_segment &segment, const std::string &text) {
    return km2::lsp::text_range {};
}
