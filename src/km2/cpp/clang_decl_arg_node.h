#ifndef CLANG_DECL_ARG_NODE_H
#define CLANG_DECL_ARG_NODE_H

#include "abstract_clang_node.h"
#pragma clang diagnostic push

#include <clang/AST/Decl.h>
#pragma clang diagnostic pop

#include <src/km2/tree/decl_arg_node.h>

class clang_decl_arg_node : public abstract_clang_node<clang::ParmVarDecl, km2::decl_arg_node> {
public:
    clang_decl_arg_node(const clang::ParmVarDecl* decl) : abstract_clang_node(decl) {};

    virtual target_ptr_type km2_node() const override;
};

#endif // CLANG_DECL_ARG_NODE_H
