#include "clang_namespace_node.h"
#include "cpp_consumer.h"

#include <llvm/AsmParser/Parser.h>
#include <iostream>
#include <clang/AST/ASTContext.h>

cpp_consumer::cpp_consumer(clang::CompilerInstance *compiler_instance, const std::string &file, std::map<std::string, clang_namespace_node> *nodes)
    : m_compiler_instance(compiler_instance),
      m_file(file),
      m_nodes(nodes) {}

void cpp_consumer::HandleTranslationUnit(clang::ASTContext &context) {
    std::cout << "CLANG COMPILATION" << std::endl;
    if(!m_nodes)
        return;

    for(const auto& d : context.getTranslationUnitDecl()->decls()) {
        if((m_nodes->operator[](m_file) = dynamic_cast<clang::NamespaceDecl*>(d))) {
            std::cout << "\tconsumer success: " << m_file << " : " << m_nodes->operator[](m_file).origin << std::endl;
            break;
        }
    }
    std::cout << "CLANG COMPILATION END" << std::endl;
}
