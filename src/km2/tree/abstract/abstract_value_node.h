#ifndef ABSTRACT_VALUE_NODE_H
#define ABSTRACT_VALUE_NODE_H

#include "abstract_node.h"
#include <wall_e/src/either.h>
#include <memory>

namespace llvm { class Value; }

namespace km2 {

class module;

class abstract_value_node : public km2::abstract_node {
public:
    typedef abstract_node super_type;
    abstract_value_node(const wall_e::text_segment& segment = {});

    virtual wall_e::either<km2::error, llvm::Value*> generate_llvm(const std::shared_ptr<module> &module) = 0;
};

} // namespace km2

#endif // ABSTRACT_VALUE_NODE_H
