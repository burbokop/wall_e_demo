#ifndef CLANG_FUNCTION_NODE_H
#define CLANG_FUNCTION_NODE_H

#include "abstract_clang_node.h"
#pragma clang diagnostic push

#include <clang/AST/Decl.h>
#pragma clang diagnostic pop

#include <src/km2/tree/function_node.h>



class clang_function_node : public abstract_clang_node<clang::FunctionDecl, km2::function_node> {
public:
    clang_function_node(const clang::FunctionDecl* decl) : abstract_clang_node(decl) {}

    virtual target_ptr_type km2_node() const override;
};

#endif // CLANG_FUNCTION_NODE_H
