#include "cpp_parse_action.h"


cpp_parse_action::cpp_parse_action(std::map<std::string, clang_namespace_node>* nodes)
    : m_nodes(nodes) {}

std::unique_ptr<clang::ASTConsumer> cpp_parse_action::CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef file) {
    return std::make_unique<cpp_consumer>(&CI, file.str(), m_nodes);
}
