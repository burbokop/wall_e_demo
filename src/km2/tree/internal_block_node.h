#ifndef INTERNAL_BLOCK_NODE_H
#define INTERNAL_BLOCK_NODE_H



#include "stmt_node.h"

namespace km2 {

class internal_block_node : public km2::abstract_value_node {
    std::shared_ptr<stmt_node> m_stmt;
    std::shared_ptr<internal_block_node> m_next_node;
public:
    typedef abstract_value_node super_type;

    internal_block_node(std::shared_ptr<stmt_node> stmt, std::shared_ptr<internal_block_node> next_node = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);


    // node interface
public:
    virtual wall_e::either<km2::error, llvm::Value*> generate_llvm(const std::shared_ptr<module> &module) override;
    virtual void print(size_t level, std::ostream &stream) override;

    // abstract_node interface
public:
    virtual std::list<error> errors() override;
};

} // namespace km2

#endif // INTERNAL_BLOCK_NODE_H
