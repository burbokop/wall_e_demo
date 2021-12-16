#ifndef CPP_CONSUMER_H
#define CPP_CONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "cpp_visitor.h"
#include <clang/Frontend/CompilerInstance.h>

class cpp_consumer : public clang::ASTConsumer {
    cpp_visitor m_visitor;
public:
    // override the constructor in order to pass CI
    explicit cpp_consumer(clang::CompilerInstance *compiler_instance);

    // override this to call our ExampleVisitor on the entire source file
    virtual void HandleTranslationUnit(clang::ASTContext &context);

    /*
    // override this to call our ExampleVisitor on each top-level Decl
    virtual bool HandleTopLevelDecl(DeclGroupRef DG) {
        // a DeclGroupRef may have multiple Decls, so we iterate through each one
        for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; i++) {
            Decl *D = *i;
            visitor->TraverseDecl(D); // recursively visit each AST node in Decl "D"
        }
        return true;
    }
*/
};





#endif // CPP_CONSUMER_H
