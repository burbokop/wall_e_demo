#ifndef INTERNAL_BLOCK_NODE_H
#define INTERNAL_BLOCK_NODE_H



#include "cmd_node.h"

namespace km2 {

class internal_block_node : public km2::abstract_value_node {
    std::shared_ptr<cmd_node> m_cmd;
    std::shared_ptr<internal_block_node> m_next_node;
public:
    typedef abstract_value_node super_type;

    internal_block_node(std::shared_ptr<cmd_node> cmd, std::shared_ptr<internal_block_node> next_node = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);


    // node interface
public:
    virtual wall_e::either<km2::error, llvm::Value*> generate_llvm(module_builder *builder) override;
    virtual void print(size_t level, std::ostream &stream) override;

    // abstract_node interface
public:
    virtual std::list<error> errors() override;
};

} // namespace km2

#endif // INTERNAL_BLOCK_NODE_H
