#ifndef KM2_BLOCK_NODE_H
#define KM2_BLOCK_NODE_H

#include "stmt_node.h"
#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class block_node : public km2::abstract_value_node {
    const std::shared_ptr<stmt_node> m_stmt;
    const std::shared_ptr<block_node> m_next_node;
public:
    typedef abstract_value_node super_type;

    block_node(const std::shared_ptr<stmt_node> &stmt, const std::shared_ptr<block_node>& next_node = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args);

    // node interface
public:
    virtual wall_e::either<wall_e::error, llvm::Value*> generate_llvm(const std::shared_ptr<module> &module) override;
    virtual void print(size_t level, std::ostream &stream) override;

    // abstract_node interface
public:
    virtual std::list<wall_e::error> errors() override;
};

} // namespace km2

#endif // KM2_BLOCK_NODE_H
