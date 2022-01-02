#ifndef KM2LSP_SERVICE_H
#define KM2LSP_SERVICE_H

#include <optional>
#include <string>
#include <map>
#include <list>
#include "../error.h"


#if __has_include(<src/km2/km2.h>)
    #include <src/km2/km2.h>
#else
    #include <km2/km2.h>
#endif


namespace km2 {

namespace lsp {

class service {
    struct cache_item {
        std::string content;
        km2::flags flags;
        km2::compilation_result compilation_result;
    };
    typedef std::string uri_t;

    std::map<uri_t, cache_item> m_cache;

public:
    service();

    void configure(const std::string& uri, const km2::flags& flags);
    std::list<wall_e::error> changeContent(const std::string& uri, const std::string& content);
    std::optional<std::string> hover(const std::string& uri, const wall_e::text_segment::predicate& predicate);
    std::list<std::string> complete(const std::string& uri, const wall_e::text_segment::predicate& predicate);
};

}
}

#endif // KM2LSP_SERVICE_H


