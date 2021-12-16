#include "cpp_parse_action.h"
#include "cpp_parser.h"


#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"



cpp_parser::cpp_parser(int argc, char **argv)
    : m_argc(argc),
      m_argv(argv) {

}

int cpp_parser::parse() {
    llvm::cl::OptionCategory myToolCategory("my-tool options");
    clang::tooling::CommonOptionsParser op(m_argc, const_cast<const char**>(m_argv), myToolCategory);



    clang::tooling::ClangTool tool(op.getCompilations(), op.getSourcePathList());

    const auto& factory = clang::tooling::newFrontendActionFactory<cpp_parse_action>();

    // run the Clang Tool, creating a new FrontendAction (explained below)
    int result = tool.run(factory.get());

    //errs() << "\nFound " << numFunctions << " functions.\n\n";
    // print out the rewritten source code ("rewriter" is a global var.)
    //rewriter.getEditBuffer(rewriter.getSourceMgr().getMainFileID()).write(errs());
    return result;

}
