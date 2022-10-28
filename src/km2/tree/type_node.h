#ifndef KM2_TYPE_NODE_H
#define KM2_TYPE_NODE_H

#include "abstract/abstract_type_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class type_node : public km2::abstract_type_node {
public:
    typedef km2::abstract_type_node super_type;

    enum type {
        Unsigned,
        Signed,
        Float,
        Double,
        String,
        Undefined
    };

    static std::string type_string(type  t);

private:
    type m_type = Undefined;
    std::optional<uint16_t> m_bits;
public:
    static std::optional<uint16_t> parse_integer_type(const std::string& str);


    type_node(const wall_e::index &index, type t, const std::optional<uint16_t>& bits);

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index);


    // abstract_node interface
public:
    virtual void print(size_t level, std::ostream &stream) const override;
    virtual void short_print(std::ostream &stream) const override;
    virtual std::list<wall_e::error> errors() const override;

    // abstract_type_node interface
public:
    virtual wall_e::either<wall_e::error, llvm::Type*> generate_llvm(const std::shared_ptr<translation_unit> &unit) override;
};

} // namespace km2

#endif // KM2_TYPE_NODE_H
