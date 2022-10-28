#ifndef CLANG_TYPE_NODE_H
#define CLANG_TYPE_NODE_H

#include "abstract_clang_node.h"

#pragma clang warning(push, 0)
#include <clang/AST/Type.h>
#pragma clang warning(pop)

#include <src/km2/tree/type_node.h>


class clang_type_node : public abstract_clang_node<clang::Type, km2::type_node> {
public:
    clang_type_node(const clang::Type* type) : abstract_clang_node(type) {}

    virtual target_ptr_type km2_node() const override;

};

#endif // CLANG_TYPE_NODE_H
