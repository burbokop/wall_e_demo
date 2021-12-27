#include "cpp_parse_action.h"
#include "cpp_parser.h"


#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Driver/Options.h"
//#include "clang/AST/AST.h"
//#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
//#include "clang/AST/RecursiveASTVisitor.h"
//#include "clang/Frontend/ASTConsumers.h"
//#include "clang/Frontend/FrontendActions.h"
//#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
//#include "clang/Rewrite/Core/Rewriter.h"

#include <iostream>

cpp_parser::cpp_parser(int argc, char **argv)
    : m_argc(argc),
      m_argv(argv) {

}

cpp_parser::result cpp_parser::parse(const std::string& code, const std::string &file_name) {
    std::map<std::string, clang_namespace_node> result;
    const int status = clang::tooling::runToolOnCode(std::make_unique<cpp_parse_action>(&result), code, file_name);

    for(const auto& m : result) {
        std::cout << "file: " << m.first << " : " << m.second.origin << std::endl;
    }

    return cpp_parser::result { result[file_name], status };
}
