#ifndef ABSTRACT_NODE_H
#define ABSTRACT_NODE_H

#include <wall_e/src/models/error.h>
#include <list>
#include <functional>
#include <wall_e/src/private/gram_private.h>

namespace km2 {

struct abstract_node {
    typedef std::function<wall_e::gram::argument(const wall_e::gram::arg_vector &)> factory;
    typedef std::vector<std::shared_ptr<abstract_node>> children_t;
private:

    const wall_e::text_segment m_segment;
    km2::abstract_node* m_parent = nullptr;
    const children_t m_children;

    mutable std::map<std::size_t, abstract_node*> m_ancestor_cache;

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

    abstract_node(const children_t &children, const wall_e::text_segment& segment = {});


    abstract_node* parent() const;


    template<typename T>
    static std::vector<T> concat_vectors(const std::vector<std::vector<T>>& vecs) {
        std::vector<T> result;
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
    static children_t cast_to_children(const std::vector<std::shared_ptr<T>>& ...vecs) {
        return concat_vectors(std::vector<children_t> { __cast_to_children(vecs) ... });
    }

    template<typename T>
    T* parent_as() const { return dynamic_cast<T*>(parent()); }

    template<typename T>
    T* nearest_ancestor() const {
        const auto& hash = typeid (T).hash_code();
        const auto& it = m_ancestor_cache.find(hash);
        if(it != m_ancestor_cache.end()) {
            return dynamic_cast<T*>(it->second);
        }

        if(const auto& p = parent()) {
            if(const auto& casted_p = dynamic_cast<T*>(p)) {
                m_ancestor_cache[hash] = p;
                return casted_p;
            } else if(const auto ancestor = p->nearest_ancestor<T>()) {
                m_ancestor_cache[hash] = ancestor;
                return ancestor;
            }
        }
        return nullptr;
    }

    wall_e::text_segment segment() const;
    std::map<std::size_t, abstract_node*> ancestor_cache() const;
    children_t children() const;

    virtual void print(size_t level, std::ostream &stream) = 0;
    virtual std::list<wall_e::error> errors() = 0;
};

} // namespace km2

#endif // ABSTRACT_NODE_H
