#ifndef CPP_CONSUMER_H
#define CPP_CONSUMER_H

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#pragma clang diagnostic pop

#include "clang_namespace_node.h"

class cpp_consumer : public clang::ASTConsumer {
    clang::CompilerInstance *m_compiler_instance;
    std::string m_file;
    std::map<std::string, clang_namespace_node> *m_nodes;
public:
    explicit cpp_consumer(clang::CompilerInstance *compiler_instance, const std::string& file, std::map<std::string, clang_namespace_node> *nodes);

    virtual void HandleTranslationUnit(clang::ASTContext &context);
};





#endif // CPP_CONSUMER_H
