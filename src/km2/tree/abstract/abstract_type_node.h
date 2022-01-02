#ifndef ABSTRACT_TYPE_NODE_H
#define ABSTRACT_TYPE_NODE_H

#include "abstract_node.h"
#include <wall_e/src/models/either.h>
#include <memory>

namespace llvm { class Type; }

namespace km2 {

class module;

class abstract_type_node : public abstract_node {
protected:
    abstract_type_node(const children_t &children, const wall_e::text_segment &segment = {});
public:
    typedef abstract_node super_type;

    virtual wall_e::either<wall_e::error, llvm::Type*> generate_llvm(const std::shared_ptr<module> &module) = 0;
};

} // namespace km2

#endif // ABSTRACT_TYPE_NODE_H
