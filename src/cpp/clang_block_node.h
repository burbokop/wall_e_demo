#ifndef CLANG_BLOCK_NODE_H
#define CLANG_BLOCK_NODE_H

#include "abstract_clang_node.h"

#include <clang/AST/Decl.h>

#include <src/km2/tree/block_node.h>



class clang_block_node : public abstract_clang_node<clang::BlockDecl, km2::block_node> {
public:
    clang_block_node(const clang::BlockDecl* decl)
        : abstract_clang_node(decl) {}

    virtual target_ptr_type km2_node() const override;
};

#endif // CLANG_BLOCK_NODE_H
