#ifndef KM2_TYPE_NODE_H
#define KM2_TYPE_NODE_H

#include "abstract/abstract_type_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class type_node : public km2::abstract_type_node {
public:
    typedef km2::abstract_type_node super_type;

    wall_e_enum_member(type,
        Unsigned,
        Signed,
        Float,
        Double,
        String,
        Undefined
    )

private:
    const type m_type = Undefined;
    const std::string m_original_text;
    const wall_e::opt<uint16_t> m_bits;
public:
    static wall_e::opt<uint16_t> parse_integer_type(const std::string& str);


    type_node(const wall_e::index &index,
            type t,
            const wall_e::opt<uint16_t> &bits,
            const std::string& original_text,
            const wall_e::text_segment &segment = {}
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index, const wall_e::gram::environment* env);

    km2::markup_string hover() const;

    // abstract_type_node interface
public:
    virtual wall_e::either<wall_e::error, backend::type*> generate_backend_type(const std::shared_ptr<backend::unit> &unit) override;

    // abstract_node interface
public:
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual wall_e::list<ast_token> tokens() const override;
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context& ctx) const override;
};

} // namespace km2

#endif // KM2_TYPE_NODE_H
