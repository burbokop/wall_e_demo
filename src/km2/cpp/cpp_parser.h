#ifndef CPP_PARSER_H
#define CPP_PARSER_H

#ifdef CLANG_ENABLED
#include "clang_namespace_node.h"


class cpp_parser {
    int m_argc = 0;
    char **m_argv = nullptr;
public:
    struct result {
        const clang_namespace_node node;
        const int status;
    };

    cpp_parser(int argc, char **argv);

    result parse(const std::string &code, const std::string& file_name);
};

#endif

#endif // CPP_PARSER_H
