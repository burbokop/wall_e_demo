#ifndef BASE_NODE_H
#define BASE_NODE_H

#include "block_node.h"
#include "node.h"
#include <wall_e/src/gram.h>
#include <string>

namespace km2 {
class base_node : public node {
    block_node *m_block_node = nullptr;
public:
    base_node(block_node *block_node = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);

    // node interface
public:
    virtual llvm::Value *generate_llvm(module_builder *builder) override;
    virtual void print(size_t level, std::ostream &stream) override;
};
}


#endif // BASE_NODE_H
