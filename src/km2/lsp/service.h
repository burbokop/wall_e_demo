#ifndef KM2LSP_SERVICE_H
#define KM2LSP_SERVICE_H

#include <optional>
#include <string>
#include <map>
#include <list>
#include "../error.h"

namespace km2 {
namespace lsp {

class service {
    std::map<std::string, std::string> m_cache;
public:
    service();

    std::list<km2::error> changeContent(const std::string& uri, const std::string& content);

    std::optional<std::string> hover(const std::string& uri, std::size_t line, std::size_t character);
    std::optional<std::string> hover(const std::string& uri, std::size_t offset);
};

}
}

#endif // KM2LSP_SERVICE_H


