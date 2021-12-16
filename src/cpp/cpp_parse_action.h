#ifndef CPP_PARSE_ACTION_H
#define CPP_PARSE_ACTION_H

#include "clang/Frontend/FrontendAction.h"

#include "cpp_consumer.h"

class cpp_parse_action : public clang::ASTFrontendAction {
public:
    cpp_parse_action();
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, clang::StringRef file) override;
};



#endif // CPP_PARSE_ACTION_H
