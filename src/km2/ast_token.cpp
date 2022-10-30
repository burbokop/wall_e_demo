#include "ast_token.h"

#include <sstream>

std::string km2::to_string(const km2::ast_token_list &tokens) {
    std::stringstream ss;

    std::size_t i = 0;
    for(const auto& t : tokens) {
        ss << t;
        if(i++ < tokens.size() - 1) ss << std::endl;
    }
    return ss.str();
}
