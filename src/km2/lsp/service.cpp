#include "service.h"

km2::lsp::service::service() {

}

std::list<km2::error> km2::lsp::service::changeContent(const std::string &uri, const std::string &content) {
    m_cache[uri] = content;
    return {
        error("some err1", wall_e::text_segment(10, 12)),
        error("some err2", wall_e::text_segment(20, 22)),
        error("some err3", wall_e::text_segment(30, 33))
    };
}

std::optional<std::string> km2::lsp::service::hover(const std::string &uri, std::size_t line, std::size_t character) {
    return std::nullopt;
}

std::optional<std::string> km2::lsp::service::hover(const std::string &uri, std::size_t offset) {
    return std::nullopt;
}

