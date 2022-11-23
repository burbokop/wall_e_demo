#ifndef ABSTRACT_NODE_H
#define ABSTRACT_NODE_H

#include <wall_e/src/models/error.h>
#include <list>
#include <functional>
#include <wall_e/src/private/gram_private.h>
#include <wall_e/src/utility/box.h>
#include <wall_e/src/utility/tree_stream.h>
#include "../../backend/models/context.h"
#include "../../ast_token.h"
#include "src/km2/utility/lvalue.h"

namespace km2 {


struct abstract_node;

template <typename T>
concept concept_node = std::is_base_of<abstract_node, T>::value;

struct node_trait {
    node_trait() {}
    virtual ~node_trait() {}
};

template <typename T>
concept concept_node_trait = std::is_base_of<node_trait, T>::value;

template<typename T, typename A>
concept tree_searcher = requires(T t) {
    { t.enter_level(std::declval<const abstract_node*>()) } -> std::convertible_to<wall_e::vec<std::shared_ptr<const abstract_node>>>;
    { t.valuable_ancestor(std::declval<const abstract_node*>()) } -> std::convertible_to<const A*>;
    { t.until() } -> std::convertible_to<const abstract_node*>;
};

wall_e_enum(semantic_error,
    DoubleExport,
    FunctionNotFound
)

struct abstract_node {
    typedef std::function<wall_e::gram::argument(const wall_e::gram::arg_vector &, const wall_e::index&, const wall_e::gram::environment*)> factory;
    typedef wall_e::vec<std::shared_ptr<const abstract_node>> children_t;
private:

    static std::vector<backend::context> contexts(const children_t& children);

    const wall_e::gram::environment* m_env;
    const wall_e::text_segment m_segment;
    mutable km2::abstract_node* m_parent = nullptr;
    const children_t m_children;

    mutable const abstract_node* m_root_cache = nullptr;
    mutable std::map<std::size_t, const abstract_node*> m_ancestor_cache;
    const backend::context m_context;
    const wall_e::index m_index;
    const wall_e::box_list<node_trait> m_traits;

    template<typename T>
    static children_t __cast_to_children(const std::vector<std::shared_ptr<T>>& vecs) {
        children_t result;
        result.reserve(vecs.size());
        for(const auto v : vecs) {
            result.push_back(std::dynamic_pointer_cast<abstract_node>(v));
        }
        return result;
    }
public:
    const wall_e::box_list<node_trait>& traits() const { return m_traits; };

    template<concept_node_trait T>
    inline const T* trait() const {
        const auto& it = std::find_if(m_traits.begin(), m_traits.end(), [](const node_trait* t) { return dynamic_cast<const T*>(t); });
        return it != m_traits.end() ? dynamic_cast<const T*>(*it) : nullptr;
    }

    template<concept_node_trait T>
    inline bool has() const { return trait<T>(); }

    const wall_e::gram::environment* env() const { return m_env; };

    abstract_node(
            const wall_e::gram::environment* env,
            const wall_e::index& index,
            const children_t &children,
            const wall_e::text_segment& segment = {},
            const wall_e::box_list<node_trait>::factory& traits = {}
            );

    inline const abstract_node* parent() const { return m_parent; }
    inline const abstract_node* root() const {
        if(!m_root_cache) {
            if(const auto* p = parent()) {
                m_root_cache = p->root();
            } else {
                m_root_cache = this;
            }
        }
        return m_root_cache;
    }

    template<typename T>
    static wall_e::vec<T> concat_vectors(const wall_e::vec<wall_e::vec<T>>& vecs) {
        wall_e::vec<T> result;
        std::size_t rs = 0;
        for(const auto& v : vecs) { rs += v.size(); }
        result.reserve(rs);
        for(const auto v : vecs) {
            for(const auto item : v) {
                result.push_back(item);
            }
        }
        return result;
    }

    template<typename ...T>
    static children_t cast_to_children(const wall_e::vec<std::shared_ptr<T>>& ...vecs) {
        return concat_vectors(wall_e::vec<children_t> { __cast_to_children(vecs) ... });
    }

    template<typename T>
    const T* parent_as() const { return dynamic_cast<const T*>(parent()); }

    /**
     * @brief nearest_ancestor
     * @param break_other - break recursion and return nullptr if parent type neither T nor this type
     * @return
     */
    template<typename T>
    const T* nearest_ancestor(bool break_other = false) const {
        const auto& hash = typeid (T).hash_code();
        const auto& it = m_ancestor_cache.find(hash);
        if(it != m_ancestor_cache.end()) {
            return dynamic_cast<const T*>(it->second);
        }

        if(const auto& p = parent()) {
            if(const auto& casted_p = dynamic_cast<const T*>(p)) {
                m_ancestor_cache[hash] = p;
                return casted_p;
            }
            if(break_other && !dynamic_cast<std::remove_const<decltype (this)>::type>(p)) {
                return nullptr;
            }
            if(const auto ancestor = p->nearest_ancestor<T>(break_other)) {
                m_ancestor_cache[hash] = ancestor;
                return ancestor;
            }
        }
        return nullptr;
    }

    wall_e::text_segment segment() const;
    std::map<std::size_t, const abstract_node*> ancestor_cache() const;
    const children_t& children() const { return m_children; }

    wall_e::opt<lvalue> lval() const;
    wall_e::str_list lval_full_name() const;



    template<typename T>
    class default_tree_searcher {
        const abstract_node* m_until;
    public:
        default_tree_searcher(const abstract_node* until = nullptr) : m_until(until) {}

        inline const abstract_node* until() const { return m_until; }
        inline wall_e::vec<std::shared_ptr<const abstract_node>> enter_level(const abstract_node* node) { return node->m_children; }
        inline const T* valuable_ancestor(const abstract_node* node) const {
            if(const auto& i = dynamic_cast<const T*>(node)) { return i; } else { return nullptr; }
        }
    };

    /**
     * @brief find - find node in posterity
     * @param predicate
     */
    template <typename T, tree_searcher<abstract_node> S = default_tree_searcher<abstract_node>>
    inline std::shared_ptr<const T> find(
                const std::function<bool(const std::shared_ptr<const T>&)>& predicate,
                S s = S {}
            ) const {
        return find<T, abstract_node, S>([predicate](const std::shared_ptr<const T>& v, const abstract_node*){ return predicate(v); }, s);
    }

    /**
     * @brief find - find node in posterity
     * @param predicate
     */
    template <typename T, typename V, tree_searcher<V> S = default_tree_searcher<V>>
    inline std::shared_ptr<const T> find(
                const std::function<bool(const std::shared_ptr<const T>&, const V*)>& predicate,
                S s = S {}
            ) const {
        return __find<T, V, S>(predicate, nullptr, s).value_or(nullptr);
    }
private:
    template <typename T, typename V, tree_searcher<V> S = default_tree_searcher<V>>
    wall_e::opt<std::shared_ptr<const T>> __find(
                const std::function<bool(const std::shared_ptr<const T>&, const V*)>& predicate,
                const V* prev_va,
                S s
            ) const {
        auto va = s.valuable_ancestor(this);
        va = va ? va : prev_va;

        for(const auto& c : s.enter_level(this)) {
            if(s.until() && c.get() == s.until()) {
                return std::nullopt;
            }
            if(const auto& cc = std::dynamic_pointer_cast<const T>(c)) {
                if(predicate(cc, va)) {
                    return cc;
                }
            }
            if(c) {
                const auto& f = c->__find(predicate, va, s);
                if(f ? bool(*f) : true) {
                    return f;
                }
            }
        }
        return nullptr;
    }
public:
    /**
     * @brief find_until - find node in whole tree until this node
     * @param predicate
     * @return
     */
    template <typename T, typename V, tree_searcher<V> S = default_tree_searcher<V>>
    inline std::shared_ptr<const T> find_until(
                const std::function<bool(const std::shared_ptr<const T>&, const V*)>& predicate
            ) const {
        return root()->find<T, V, S>(predicate, S(this));
    }

    template <typename T, tree_searcher<abstract_node> S = default_tree_searcher<abstract_node>>
    inline std::shared_ptr<const T> find_until(
                const std::function<bool(const std::shared_ptr<const T>&)>& predicate
            ) const {
        return find_until<T, abstract_node, S>([predicate](const std::shared_ptr<const T>& v, const abstract_node*){ return predicate(v); });
    }

    virtual std::ostream& write(std::ostream &stream, const wall_e::tree_writer::context& ctx) const = 0;
    virtual std::ostream& short_print(std::ostream &stream) const = 0;
    virtual ast_token_list tokens() const = 0;
    virtual ast_token_type rvalue_type() const = 0;
    virtual markup_string hover() const = 0;
    virtual bool is_export_root() const { return false; };

    //km2::abstract_node

    template<concept_node T>
    inline static ast_token_list tokens_from_node_list(const std::vector<std::shared_ptr<T>>& nodes) {
        ast_token_list result;
        for(const auto& n : nodes) {
            if(n) {
                const auto& t = n->tokens();
                result.insert(result.end(), t.begin(), t.end());
            }
        }
        return result;
    }

    virtual wall_e::list<wall_e::error> errors() const = 0;
    const backend::context &ctx() const;
    const wall_e::index &index() const;

    inline friend std::ostream& operator<<(std::ostream& stream, const abstract_node* node) { node->short_print(stream); return stream; }
    inline friend std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<abstract_node>& node) {
        if(node) {
            node->short_print(stream);
            return stream;
        } else {
            return stream << "null_node";
        }
    }
};

} // namespace km2

#endif // ABSTRACT_NODE_H
