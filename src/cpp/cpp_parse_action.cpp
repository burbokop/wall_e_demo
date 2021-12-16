#include "cpp_parse_action.h"

cpp_parse_action::cpp_parse_action() {}

std::unique_ptr<clang::ASTConsumer> cpp_parse_action::CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef file) {
    return std::make_unique<cpp_consumer>(&CI);
}
