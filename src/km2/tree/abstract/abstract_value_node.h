#ifndef ABSTRACT_VALUE_NODE_H
#define ABSTRACT_VALUE_NODE_H

#include "abstract_node.h"



namespace km2 {

class abstract_value_node : public km2::abstract_node {
public:
    typedef abstract_node super_type;
    abstract_value_node();

    virtual llvm::Value *generate_llvm(module_builder *builder) = 0;
};

} // namespace km2

#endif // ABSTRACT_VALUE_NODE_H
