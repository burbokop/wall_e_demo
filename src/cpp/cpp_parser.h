#ifndef CPP_PARSER_H
#define CPP_PARSER_H


class cpp_parser {
    int m_argc = 0;
    char **m_argv = nullptr;
public:
    cpp_parser(int argc, char **argv);

    int parse();
};

#endif // CPP_PARSER_H
