#ifndef ABSTRACT_NODE_H
#define ABSTRACT_NODE_H

#include <llvm/IR/Value.h>

#include <src/km2/builder.h>

#include <src/km2/error.h>

namespace km2 {

class abstract_node {
public:
    abstract_node();

    virtual void print(size_t level, std::ostream &stream) = 0;
    virtual std::list<error> errors() = 0;
};

} // namespace km2

#endif // ABSTRACT_NODE_H
