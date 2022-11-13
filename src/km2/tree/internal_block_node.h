#ifndef INTERNAL_BLOCK_NODE_H
#define INTERNAL_BLOCK_NODE_H



#include "stmt_node.h"

namespace km2 {

/// deprecated
class internal_block_node : public km2::abstract_value_node {
    std::shared_ptr<stmt_node> m_stmt;
    std::shared_ptr<internal_block_node> m_next_node;
public:
    typedef abstract_value_node super_type;

    internal_block_node(const wall_e::index &index, std::shared_ptr<stmt_node> stmt, std::shared_ptr<internal_block_node> next_node = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);


    // node interface
public:
    virtual wall_e::either<
        wall_e::error,
        backend::value*
    > generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;

    // abstract_node interface
public:
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual std::ostream& short_print(std::ostream &stream) const override;
    virtual wall_e::list<ast_token> tokens() const override;
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const override;
};

} // namespace km2

#endif // INTERNAL_BLOCK_NODE_H
