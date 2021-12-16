#ifndef CPP_BASE_NODE_H
#define CPP_BASE_NODE_H

#include <clang/AST/Decl.h>



class cpp_base_node {
public:
    cpp_base_node(clang::TranslationUnitDecl* decl);
};

#endif // CPP_BASE_NODE_H
