#ifndef KM2_ASM_NODE_H
#define KM2_ASM_NODE_H

#include "abstract/abstract_value_node.h"
#include <wall_e/src/gram.h>

namespace km2 {

class asm_node : public km2::abstract_value_node {
    const std::string m_text;
public:
    typedef abstract_value_node super_type;

    asm_node(
            const wall_e::gram::environment* env,
            const wall_e::index &index,
            const std::string &text,
            const wall_e::text_segment& segment = {}
            );


    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index, const wall_e::gram::environment* env);

    // node interface
public:
    virtual wall_e::either<wall_e::error, backend::value*> generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;

    // abstract_node interface
public:
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual wall_e::list<ast_token> tokens() const override;
    virtual std::ostream &write(std::ostream &stream, const wall_e::tree_writer::context& ctx) const override;
    //virtual bool is_export_root() const override { return false; }
    virtual ast_token_type rvalue_type() const override;
    virtual markup_string hover() const override;
};

} // namespace km2

#endif // KM2_ASM_NODE_H
