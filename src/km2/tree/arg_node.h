#ifndef KM2_ARG_NODE_H
#define KM2_ARG_NODE_H

#include "abstract/abstract_value_node.h"

#include <wall_e/src/gram.h>

namespace km2 {

class arg_node : public km2::abstract_value_node {
public:
    typedef abstract_value_node super_type;

    enum type {
        Id,
        IntLiteral,
        FloatLiteral,
        StringLiteral,
        ValueNode,
        Undefined
    };
    static std::string type_string(const type& t);
private:
    type m_type;
    std::string m_text;
    std::shared_ptr<abstract_value_node> m_value_node;
public:

    arg_node(
            const wall_e::text_segment& segment,
            type t,
            const std::string& text = {},
            std::shared_ptr<abstract_value_node> value_node = nullptr
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);


    // node interface
public:
    virtual wall_e::either<
        wall_e::error,
        llvm::Value*
    > generate_llvm(const std::shared_ptr<translation_unit> &unit) override;
    virtual void print(size_t level, std::ostream &stream) override;    
    virtual std::list<wall_e::error> errors() const override;
};

} // namespace km2

#endif // KM2_ARG_NODE_H
