#ifndef ABSTRACT_TYPE_NODE_H
#define ABSTRACT_TYPE_NODE_H

#include "abstract_node.h"
#include <wall_e/src/either.h>

namespace llvm { class Type; }

namespace km2 {

class module_builder;

class abstract_type_node : public abstract_node {
public:
    typedef abstract_node super_type;
    abstract_type_node(const text_segment& segment = {});

    virtual wall_e::either<km2::error, llvm::Type*> generate_llvm(module_builder *builder) = 0;
};

} // namespace km2

#endif // ABSTRACT_TYPE_NODE_H
