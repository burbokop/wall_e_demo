#include "service.h"
#include "../tree/abstract/abstract_node.h"
#include "wall_e/src/macro.h"

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

km2::lsp::semantic_tokens_legend km2::lsp::service::register_semantic_tokens(const semantic_tokens_client_capability &client_cap) {
    m_semantic_token_types_map.clear();

    const auto& all_token_types = wall_e::enums::reflect_names<semantic_token_type>();
    const auto& all_token_modifiers = wall_e::enums::reflect_names<semantic_token_modifier>();

    m_semantic_token_types_map = default_semantic_token_types_map();
    m_ast_semantic_token_types_map = default_ast_semantic_token_types_map();

    m_semantic_token_modifiers_map = default_semantic_token_modifiers_map();
    m_ast_semantic_token_modifiers_map = default_ast_semantic_token_modifiers_map();

    wall_e::vec<semantic_token_type> legent_types;
    legent_types.reserve(all_token_types.size());
    for(std::size_t i = 0; i < all_token_types.size(); ++i) {
        const auto type = semantic_token_type(i);
        if(std::find(client_cap.token_types.begin(), client_cap.token_types.end(), type) != client_cap.token_types.end()) {
            legent_types.push_back(type);
        } else {
            legent_types.push_back(wall_e::enums::max_value<semantic_token_type>());
        }
    }

    wall_e::vec<semantic_token_modifier> legent_modifiers;
    legent_modifiers.reserve(all_token_modifiers.size());
    for(std::size_t i = 0; i < all_token_modifiers.size(); ++i) {
        const auto modifier = semantic_token_modifier(i);
        if(std::find(client_cap.token_modifiers.begin(), client_cap.token_modifiers.end(), modifier) != client_cap.token_modifiers.end()) {
            legent_modifiers.push_back(modifier);
        } else {
            legent_modifiers.push_back(wall_e::enums::max_value<semantic_token_modifier>());
        }
    }

    return semantic_tokens_legend {
        .token_types = legent_types,
        .token_modifiers = legent_modifiers
    };
}

bool km2::lsp::service::ast_tokens_ready(const std::string &uri) const {
    const auto cache_it = m_cache.find(uri);
    if(cache_it != m_cache.end() && cache_it->second.compilation_result) {
        if(cache_it->second.compilation_result->root_node() && m_ast_semantic_token_types_map.size() > 0) {
            return true;
        }
    }
    return false;
}

wall_e::list<wall_e::error> km2::lsp::service::change_content(const std::string &uri, const std::string &content) {
    auto& ref = m_cache[uri];
    ref.content = content;
    ref.lines = wall_e::text_segment::lines(uri, content, false);
    ref.compilation_result = km2::compile(nullptr, content, uri, m_flags, m_log);
    return ref.compilation_result->errors();
}

wall_e::vec<wall_e::lex::token> km2::lsp::service::tokens(const std::string &uri) const {
    const auto cache_it = m_cache.find(uri);
    if(cache_it != m_cache.end() && cache_it->second.compilation_result) {
        return cache_it->second.compilation_result->tokens();
    }
    return {};
}

wall_e::vec<km2::lsp::semantic_token> km2::lsp::service::semantic_tokens(const std::string& uri, bool split_multiline) const {
    const auto cache_it = m_cache.find(uri);
    if(cache_it != m_cache.end() && cache_it->second.compilation_result) {
        if(cache_it->second.compilation_result->root_node() && m_ast_semantic_token_types_map.size() > 0) {
            const auto& tokens = cache_it->second.compilation_result->ast_tokens();
            const auto& comments = cache_it->second.compilation_result->comments();
            wall_e::vec<semantic_token> result;
            result.reserve(tokens.size() + comments.size());
            for(const auto& t : tokens) {
                const auto& ttype_it = m_ast_semantic_token_types_map.find(t.type);
                if(ttype_it != m_ast_semantic_token_types_map.end()) {
                    result.push_back(semantic_token {
                        .type = ttype_it->second,
                        .modifier = m_ast_semantic_token_modifiers_map.get_or(
                                         t.modifier,
                                         wall_e::enums::max_value<semantic_token_modifier>()
                                         ),
                        .segment = t.segment
                    });
                }
            }
            for(const auto& c : comments) {
                const auto& ttype_it = m_semantic_token_types_map.find(c.name);
                if(ttype_it != m_semantic_token_types_map.end()) {
                    result.push_back(semantic_token {
                        .type = ttype_it->second,
                        .modifier = m_semantic_token_modifiers_map.get_or(
                                         c.name,
                                         wall_e::enums::max_value<semantic_token_modifier>()
                                         ),
                        .segment = c.segment()
                    });
                }
            }

            std::sort(result.begin(), result.end(), [](const semantic_token& t0, const semantic_token& t1){
                return t0.segment.begin() < t1.segment.begin();
            });

            if(m_flags.contains([](flag f) { return f == Verbose; })) {
                m_log << "lsp: semantic tokens:" << std::endl;
                for(const auto& t : result) {
                    m_log << "\n" << t << std::endl;
                }
            }
            return split_multiline ? split_multiline_sementic_tokens(result, cache_it->second.lines) : result;
        }

        /** if ast tokens not ready make semantic tokens from lex tokens */ {
            const auto& tokens = cache_it->second.compilation_result->tokens();
            wall_e::vec<semantic_token> result;
            result.reserve(tokens.size());
            for(const auto& t : tokens) {
                const auto& ttype_it = m_semantic_token_types_map.find(t.name);
                if(ttype_it != m_semantic_token_types_map.end()) {
                    result.push_back(semantic_token {
                        .type = ttype_it->second,
                        .modifier = m_semantic_token_modifiers_map.get_or(
                                         t.name,
                                         wall_e::enums::max_value<semantic_token_modifier>()
                                         ),
                        .segment = t.segment()
                    });
                }
            }
            return split_multiline ? split_multiline_sementic_tokens(result, cache_it->second.lines) : result;
        }
    }
    return wall_e::vec<semantic_token> {};
}

wall_e::opt<km2::markup_string> km2::lsp::service::hover(const std::string& uri, const wall_e::text_segment::predicate &predicate) const {
    const auto cache_it = m_cache.find(uri);
    if(cache_it != m_cache.end() && cache_it->second.compilation_result) {
        const auto& hovers = cache_it->second.compilation_result->hovers();
        const auto it = std::find_if(hovers.begin(), hovers.end(), [&predicate, &cache_it](
                                     const std::pair<wall_e::text_segment, markup_string>& pair
                                     ){
            return predicate(pair.first);
        });
        if(it != hovers.end()) {
            return it->second;
        }
    }
    return std::nullopt;
}

wall_e::list<std::string> km2::lsp::service::complete(const std::string& uri, const wall_e::text_segment::predicate &predicate) {
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
    return { "gogadoda" };
}




wall_e::vec<km2::lsp::semantic_token_type> km2::lsp::parse_semantic_token_types(const wall_e::vec<std::string> &toks) {
    wall_e::vec<km2::lsp::semantic_token_type> result;
    result.reserve(toks.size());
    for(const auto& t : toks) {
        if(const auto& r = wall_e::enums::from_string<km2::lsp::semantic_token_type>(first_char_to_upper(t))) {
            result.push_back(*r);
        }
    }
    return result;
}

wall_e::vec<km2::lsp::semantic_token_modifier> km2::lsp::parse_semantic_token_modifiers(const wall_e::vec<std::string> &modys) {
    wall_e::vec<km2::lsp::semantic_token_modifier> result;
    result.reserve(modys.size());
    for(const auto& t : modys) {
        if(const auto& r = wall_e::enums::from_string<km2::lsp::semantic_token_modifier>(first_char_to_upper(t))) {
            result.push_back(*r);
        }
    }
    return result;
}

wall_e::vec<std::string> km2::lsp::stringify_semantic_token_types(const wall_e::vec<semantic_token_type> &toks) {
    wall_e::vec<std::string> result;
    result.reserve(toks.size());
    for(const auto& t : toks) {
        result.push_back(first_char_to_lower(wall_e::enums::to_string(t)));
    }
    return result;
}

wall_e::vec<std::string> km2::lsp::stringify_semantic_token_modifiers(const wall_e::vec<semantic_token_modifier> &modys) {
    wall_e::vec<std::string> result;
    result.reserve(modys.size());
    for(const auto& t : modys) {
        result.push_back(first_char_to_lower(wall_e::enums::to_string(t)));
    }
    return result;
}

wall_e::vec<km2::lsp::semantic_token> km2::lsp::split_multiline_sementic_tokens(
        const wall_e::vec<semantic_token> &tokens,
        const wall_e::list<wall_e::text_segment>& lines
        ) {
    wall_e::vec<km2::lsp::semantic_token> result;
    for(const auto& t : tokens) {
        for(const auto s : t.segment.split_by_mask(lines)) {
            result.push_back(semantic_token { .type = t.type, .modifier = t.modifier, .segment = s });
        }
    }
    return result;
}
