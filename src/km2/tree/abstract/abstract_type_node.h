#ifndef ABSTRACT_TYPE_NODE_H
#define ABSTRACT_TYPE_NODE_H

#include "abstract_node.h"
#include <wall_e/src/either.h>
#include <memory>

namespace llvm { class Type; }

namespace km2 {

class module;

class abstract_type_node : public abstract_node {
public:
    typedef abstract_node super_type;
    abstract_type_node(const wall_e::text_segment& segment = {});

    virtual wall_e::either<km2::error, llvm::Type*> generate_llvm(const std::shared_ptr<module> &module) = 0;
};

} // namespace km2

#endif // ABSTRACT_TYPE_NODE_H
