#ifndef KM2_NODE_H
#define KM2_NODE_H

#include <llvm/IR/Value.h>

#include <src/km2/builder.h>

namespace km2 {

class node {
public:
    node();

    virtual llvm::Value *generate_llvm(module_builder *builder) = 0;
    virtual void print(size_t level, std::ostream &stream) = 0;
};

} // namespace km2

#endif // KM2_NODE_H
