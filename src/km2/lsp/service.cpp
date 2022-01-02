#include "service.h"


km2::lsp::service::service() {

}

void km2::lsp::service::configure(const std::string &uri, const flags &flags) {
    m_cache[uri].flags = flags;
}

std::list<wall_e::error> km2::lsp::service::changeContent(const std::string &uri, const std::string &content) {
    auto& ref = m_cache[uri];
    ref.content = content;
    ref.compilation_result = km2::compile(content, ref.flags);
    return ref.compilation_result.errors;
}

std::optional<std::string> km2::lsp::service::hover(const std::string &uri, const wall_e::text_segment::predicate &predicate) {
    const auto cache_it = m_cache.find(uri);
    if(cache_it != m_cache.end()) {

        const auto& hovers = cache_it->second.compilation_result.hovers;
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


