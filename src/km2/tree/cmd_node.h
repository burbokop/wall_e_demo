#ifndef KM2_CMD_NODE_H
#define KM2_CMD_NODE_H

#include "node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class cmd_node : public km2::node {
    node* m_node;
public:
    cmd_node(node *n = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);

    // node interface
public:
    virtual llvm::Value *generate_llvm(module_builder *builder) override;
    virtual void print(size_t level, std::ostream &stream) override;
};

} // namespace km2

#endif // KM2_CMD_NODE_H
