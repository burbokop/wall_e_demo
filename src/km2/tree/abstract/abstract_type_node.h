#ifndef ABSTRACT_TYPE_NODE_H
#define ABSTRACT_TYPE_NODE_H

#include "abstract_node.h"

namespace km2 {

class abstract_type_node : public abstract_node {
public:
    typedef abstract_node super_type;
    abstract_type_node();

    virtual llvm::Type *generate_llvm(module_builder *builder) = 0;
};

} // namespace km2

#endif // ABSTRACT_TYPE_NODE_H
