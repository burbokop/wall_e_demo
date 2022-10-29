#ifndef KM2_CONST_NODE_H
#define KM2_CONST_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class arg_node;

class const_node : public km2::abstract_value_node {
    const std::string m_id;
    const wall_e::text_segment m_id_segment;
    const std::shared_ptr<arg_node> m_value;
public:
    typedef abstract_value_node super_type;

    const_node(
            const wall_e::index &index,
            const std::string &id,
            const wall_e::text_segment& id_segment,
            const std::shared_ptr<arg_node>& value = nullptr
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);

    // abstract_value_node interface
public:
    virtual wall_e::either<wall_e::error, llvm::Value *> generate_llvm(const std::shared_ptr<translation_unit> &unit) override;

    // abstract_node interface
public:
    virtual void print(size_t level, std::ostream &stream) const override;
    virtual void short_print(std::ostream &stream) const override;
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual wall_e::list<ast_token> tokens() const override;
};

} // namespace km2

#endif // KM2_CONST_NODE_H
