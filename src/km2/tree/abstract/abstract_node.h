#ifndef ABSTRACT_NODE_H
#define ABSTRACT_NODE_H

#include <wall_e/src/models/error.h>
#include <list>
#include <functional>
#include <wall_e/src/private/gram_private.h>
#include <src/km2/translation_unit/models/context.h>
#include <wall_e/src/enums.h>

namespace km2 {

wall_e_enum(ast_token_type,
    AstNamespace = 0,
    AstType,
    AstClass,
    AstEnum,
    AstInterface,
    AstStruct,
    AstTypeParameter,
    AstParameter,
    AstVariable,
    AstProperty,
    AstEnumMember,
    AstEvent,
    AstFunction,
    AstMethod,
    AstMacro,
    AstKeyword,
    AstModifier,
    AstComment,
    AstString,
    AstNumber,
    AstRegexp,
    AstOperator,
    AstDecorator
)

struct ast_token {
    ast_token_type type;
    std::string node_type;
    std::string comment;
    std::string text;
    wall_e::text_segment segment;

    friend inline std::ostream& operator<<(std::ostream& stream, const ast_token& token) {
        return stream << "{ type: " << token.type
                      << ", node_type: " << token.node_type
                      << ", comment: " << token.comment
                      << ", text: " << token.text
                      << ", segment: " << token.segment
                      << " }";
    }
};

typedef wall_e::list<ast_token> ast_token_list;

std::string to_string(const std::list<ast_token>& tokens);

struct abstract_node;

template <typename T>
concept concept_node = std::is_base_of<abstract_node, T>::value;

struct abstract_node {
    typedef std::function<wall_e::gram::argument(const wall_e::gram::arg_vector &, const wall_e::index&)> factory;
    typedef std::vector<std::shared_ptr<abstract_node>> children_t;

private:

    static std::vector<context> contexts(const children_t& children);

    const wall_e::text_segment m_segment;
    km2::abstract_node* m_parent = nullptr;
    const children_t m_children;
    mutable std::map<std::size_t, abstract_node*> m_ancestor_cache;
    const km2::context m_context;
    const wall_e::index m_index;

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
    abstract_node(
            const wall_e::index& index,
            const children_t &children,
            const wall_e::text_segment& segment = {}
            );

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

    /**
     * @brief nearest_ancestor
     * @param break_other - break recursion and return nullptr if parent type neither T nor this type
     * @return
     */
    template<typename T>
    T* nearest_ancestor(bool break_other = false) const {
        const auto& hash = typeid (T).hash_code();
        const auto& it = m_ancestor_cache.find(hash);
        if(it != m_ancestor_cache.end()) {
            return dynamic_cast<T*>(it->second);
        }

        if(const auto& p = parent()) {
            if(const auto& casted_p = dynamic_cast<T*>(p)) {
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
    std::map<std::size_t, abstract_node*> ancestor_cache() const;
    children_t children() const;

    virtual void print(size_t level, std::ostream &stream) const = 0;
    virtual void short_print(std::ostream &stream) const = 0;
    virtual ast_token_list tokens() const = 0;

    //km2::abstract_node

    template<concept_node T>
    inline static ast_token_list tokens_from_node_list(const std::vector<std::shared_ptr<T>>& nodes) {
        ast_token_list result;
        for(const auto& n : nodes) {
            const auto& t = n->tokens();
            result.insert(result.end(), t.begin(), t.end());
        }
        return result;
    }

    virtual wall_e::list<wall_e::error> errors() const = 0;
    const km2::context &ctx() const;
    const wall_e::index &index() const;

    inline friend std::ostream& operator<<(std::ostream& stream, const abstract_node* node) { node->short_print(stream); return stream; }
    inline friend std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<abstract_node>& node) { node->short_print(stream); return stream; }
};

} // namespace km2

#endif // ABSTRACT_NODE_H
