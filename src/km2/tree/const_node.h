#ifndef KM2_CONST_NODE_H
#define KM2_CONST_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class arg_node;

class const_node : public km2::abstract_value_node {
    std::string m_id;
    std::shared_ptr<arg_node> m_value;
public:
    typedef abstract_value_node super_type;

    const_node(const std::string &id, const std::shared_ptr<arg_node>& value = nullptr);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);

    // abstract_node interface
public:
    virtual void print(size_t level, std::ostream &stream) override;
    virtual std::list<wall_e::error> errors() const override;

    // abstract_value_node interface
public:
    virtual wall_e::either<wall_e::error, llvm::Value *> generate_llvm(const std::shared_ptr<translation_unit> &unit) override;
};

} // namespace km2

#endif // KM2_CONST_NODE_H
