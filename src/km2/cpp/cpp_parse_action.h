#ifndef CPP_PARSE_ACTION_H
#define CPP_PARSE_ACTION_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include "clang/Frontend/FrontendAction.h"
#pragma GCC diagnostic pop

#include "cpp_consumer.h"

class cpp_parse_action : public clang::ASTFrontendAction {
    std::map<std::string, clang_namespace_node> *m_nodes;
public:
    cpp_parse_action(std::map<std::string, clang_namespace_node>* nodes);
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, clang::StringRef file) override;
    std::map<std::string, clang_namespace_node> nodes() const;
};



#endif // CPP_PARSE_ACTION_H
