#ifndef CLANG_NAMESPACE_NODE_H
#define CLANG_NAMESPACE_NODE_H

#include "abstract_clang_node.h"

#pragma clang warning(push, 0)
#include <clang/AST/Decl.h>
#pragma clang warning(pop)

#include <src/km2/tree/namespace_node.h>

class clang_namespace_node : public abstract_clang_node<clang::NamespaceDecl, km2::namespace_node> {
public:
    clang_namespace_node(const clang::NamespaceDecl* decl = nullptr)
        : abstract_clang_node(decl) {}

    virtual target_ptr_type km2_node() const override;
};

#endif // CLANG_NAMESPACE_NODE_H
