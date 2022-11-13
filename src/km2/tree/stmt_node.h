#ifndef KM2_STMT_NODE_H
#define KM2_STMT_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>
#include <src/km2/backend/models/context.h>

namespace km2 {

class stmt_node : public km2::abstract_value_node {
    std::shared_ptr<abstract_value_node> m_node;
public:
    typedef abstract_value_node super_type;

    stmt_node(const wall_e::index &index, std::shared_ptr<abstract_value_node> node = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);

    const backend::context m_context;
    const backend::context &context() const;
public:

    // node interface
public:
    virtual wall_e::either<wall_e::error, backend::value*> generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;

    // abstract_node interface
public:
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual wall_e::list<ast_token> tokens() const override { return m_node ? m_node->tokens() : wall_e::list<ast_token> {}; }
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context& ctx) const override;
};

} // namespace km2

#endif // KM2_STMT_NODE_H
