#ifndef KM2_CALL_NODE_H
#define KM2_CALL_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class call_node : public km2::abstract_value_node {
    std::string m_name;
    std::vector<std::shared_ptr<km2::abstract_value_node>> m_args;
    wall_e::text_segment m_name_segment;
public:
    typedef abstract_value_node super_type;

    call_node(
            const wall_e::index& index,
            const std::string& name,
            const std::vector<std::shared_ptr<km2::abstract_value_node>>& args,
            const wall_e::text_segment& name_segment
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);

    // node interface
public:
    virtual wall_e::either<
        wall_e::error,
        llvm::Value*
    > generate_llvm(const std::shared_ptr<translation_unit> &unit) override;
    virtual void print(size_t level, std::ostream &stream) override;

    // abstract_node interface
public:
    virtual std::list<wall_e::error> errors() const override;
};

} // namespace km2

#endif // KM2_CALL_NODE_H
