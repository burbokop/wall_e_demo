#ifndef KM2_STMT_NODE_H
#define KM2_STMT_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>
#include <src/km2/translation_unit/models/context.h>

namespace km2 {

class stmt_node : public km2::abstract_value_node {
    std::shared_ptr<abstract_value_node> m_node;
public:
    typedef abstract_value_node super_type;

    stmt_node(const wall_e::index &index, std::shared_ptr<abstract_value_node> node = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);

    const km2::context m_context;
    // node interface
public:
    virtual wall_e::either<wall_e::error, llvm::Value*> generate_llvm(const std::shared_ptr<translation_unit> &unit) override;
    virtual void print(size_t level, std::ostream &stream) const override;

    // abstract_node interface
public:
    virtual std::list<wall_e::error> errors() const override;
    const km2::context &context() const;

    // abstract_node interface
public:
    virtual void short_print(std::ostream &stream) const override;
};

} // namespace km2

#endif // KM2_STMT_NODE_H
