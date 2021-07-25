#ifndef KM2_BLOCK_NODE_H
#define KM2_BLOCK_NODE_H

#include "cmd_node.h"
#include "node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class block_node : public km2::node {
    cmd_node* m_cmd;
    block_node *m_next_node;
public:
    block_node(cmd_node* cmd, block_node *next_node = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);

    // node interface
public:
    virtual llvm::Value *generate_llvm(module_builder *builder) override;
    virtual void print(size_t level, std::ostream &stream) override;
};

} // namespace km2

#endif // KM2_BLOCK_NODE_H
