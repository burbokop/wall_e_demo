#ifndef IMP_NODE_H
#define IMP_NODE_H

#include "abstract/abstract_value_node.h"

#include <wall_e/src/utility/box.h>
#include <wall_e/src/utility/lazy.h>

namespace km2 {

class compilation_result;

class imp_node : public abstract_value_node {
    const wall_e::lex::token m_keyword_token;
    const wall_e::lex::token m_name_token;
    const wall_e::opt_box<compilation_result> m_module_cresult;
    const wall_e::list<wall_e::error> m_search_errors;

    std::shared_ptr<const abstract_value_node> eval_mod_exp_root_cache() const;
    const wall_e::lazy<std::shared_ptr<const abstract_value_node>> m_mod_exp_root_cache = wall_e::lazy(this, &imp_node::eval_mod_exp_root_cache);
public:
    typedef abstract_value_node super_type;

    imp_node(
            const wall_e::index& index,
            const wall_e::lex::token& keyword_token,
            const wall_e::lex::token& name_token,
            const wall_e::opt<compilation_result>& module_cresult,
            const wall_e::list<wall_e::error>& search_errors
            );

    static wall_e::gram::argument create(const wall_e::gram::arg_vector &args, const wall_e::index &index, const wall_e::gram::environment* env);

    inline const std::shared_ptr<const abstract_value_node> mod_exp_root() const { return m_mod_exp_root_cache.value(); }

    // abstract_node interface
public:
    virtual std::ostream &write(std::ostream &stream, write_format fmt, const wall_e::tree_writer::context &ctx) const override;
    virtual std::ostream &short_print(std::ostream &stream) const override;
    virtual ast_token_list tokens() const override;
    virtual wall_e::list<wall_e::error> errors() const override;
    virtual ast_token_type rvalue_type() const override;
    virtual markup_string hover() const override;

    // abstract_value_node interface
public:
    virtual wall_e::either<wall_e::error, backend::value *> generate_backend_value(const std::shared_ptr<backend::unit> &unit) override;
};

/// concept tree_seracher
template<typename T>
class pub_api_searcher {
    const abstract_node* m_until;
public:
    inline pub_api_searcher(const abstract_node* until = nullptr) : m_until(until) {}

    inline const abstract_node* until() const { return m_until; }

    inline const T* valuable_ancestor(const abstract_node* node) const {
        if(const auto& i = dynamic_cast<const T*>(node)) { return i; } else { return nullptr; }
    }

    inline wall_e::vec<std::shared_ptr<const abstract_node>> enter_level(const abstract_node* node) const {
        if(!node) return {};
        if(const auto& i = dynamic_cast<const imp_node*>(node)) {
            return { i->mod_exp_root() };
        } else {
            return node->children();
        }
    }
};

}

#endif // IMP_NODE_H
