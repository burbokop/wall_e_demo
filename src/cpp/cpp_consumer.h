#ifndef CPP_CONSUMER_H
#define CPP_CONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "clang_namespace_node.h"
#include "cpp_visitor.h"
#include <clang/Frontend/CompilerInstance.h>

class cpp_consumer : public clang::ASTConsumer {
    clang::CompilerInstance *m_compiler_instance;
    std::string m_file;
    std::map<std::string, clang_namespace_node> *m_nodes;
public:
    explicit cpp_consumer(clang::CompilerInstance *compiler_instance, const std::string& file, std::map<std::string, clang_namespace_node> *nodes);

    virtual void HandleTranslationUnit(clang::ASTContext &context);
};





#endif // CPP_CONSUMER_H
