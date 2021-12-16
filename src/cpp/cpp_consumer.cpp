#include "cpp_base_node.h"
#include "cpp_consumer.h"

#include <llvm/AsmParser/Parser.h>
#include <iostream>
#include <clang/AST/ASTContext.h>

cpp_consumer::cpp_consumer(clang::CompilerInstance *compiler_instance)
    : m_visitor(compiler_instance->getSourceManager()) {}

void cpp_consumer::HandleTranslationUnit(clang::ASTContext &context) {
    /* we can use ASTContext to get the TranslationUnitDecl, which is
             a single Decl that collectively represents the entire source file */

    //clang::PrettyStackTraceDecl()

    //context.getType

    //context.getTranslationUnitDecl();



    //m_visitor.TraverseDecl(context.getTranslationUnitDecl());

    //m_visitor


    std::cout << "context1: " << &context << "ctx2: " << &(context.getTranslationUnitDecl()->getASTContext()) << std::endl;


    cpp_base_node node(context.getTranslationUnitDecl());

    const auto& rn = m_visitor.result_node();

    std::cout << "VIVITOR TREE" << std::endl;
    if(rn) {
        rn->print(0, std::cout);
    } else {
        std::cout << "nullptr" << std::endl;
    }
    std::cout << "VIVITOR TREE END" << std::endl;

    //const auto& decl = context.getTranslationUnitDecl();

    //decl->

    //llvm::outs() << decl;

    //clang::FullSourceLoc functionDeclFullLocation = context.getFullLoc(func.getLocStart());
    //    if (functionDeclFullLocation.isValid())
    //        llvm::outs() << "Found FunctionDecl at "
    //        << functionDeclFullLocation.getManager().getFilename(functionDeclFullLocation) << ":"
    //        << functionDeclFullLocation.getSpellingLineNumber() << ":"
    //        << functionDeclFullLocation.getSpellingColumnNumber() << "\n";


    //llvm::outs() << context;

    //std::cout << Context.Comments.empty() << std::endl;
}
