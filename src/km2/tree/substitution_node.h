#ifndef SUBSTITUTION_NODE_H
#define SUBSTITUTION_NODE_H

#include "abstract/abstract_value_node.h"

#include <src/km2/utility/lvalue.h>

#include <wall_e/src/utility/lazy.h>

namespace km2 {

class substitution_node : public abstract_value_node {
    const lvalue m_lvalue;
    const std::shared_ptr<abstract_value_node> m_rvalue;
    //mutable wall_e::opt<std::shared_ptr<const substitution_node>> m_prev_exp_node_cache = nullptr;

    std::shared_ptr<const substitution_node> eval_prev_exp_node() const;
    const wall_e::lazy<std::shared_ptr<const substitution_node>> m_prev_exp_node_cache = wall_e::lazy(this, &substitution_node::eval_prev_exp_node);
public:
    typedef abstract_value_node super_type;

    substitution_node(
            const wall_e::index& index,
            const struct lvalue& lvalue,
            const std::shared_ptr<abstract_value_node>& rvalue
            );

    static factory create(const lvalue::factory& lval_factory);

    const lvalue& lvalue() const { return m_lvalue; }
    const std::shared_ptr<abstract_value_node>& rvalue() const { return m_rvalue; };

    wall_e::str_list full_name() const;

    inline const std::shared_ptr<const substitution_node>& prev_exp_node() const { return m_prev_exp_node_cache.value(); }

    // abstract_node interface
public:
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const override;
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual ast_token_list tokens() const override;
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual bool is_export_root() const override;
    virtual ast_token_type rvalue_type() const override;
    virtual markup_string hover() const override;

    // abstract_value_node interface
public:
    virtual wall_e::either<wall_e::error, backend::value *> generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;
};

}

#endif // SUBSTITUTION_NODE_H
