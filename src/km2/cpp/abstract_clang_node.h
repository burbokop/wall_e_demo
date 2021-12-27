#ifndef ABSTRACT_CLANG_NODE_H
#define ABSTRACT_CLANG_NODE_H

#include <memory>


/**
 * immutable type
 */
template<typename Origin, typename Target>
struct abstract_clang_node {
    const Origin *origin;

    typedef Origin origin_type;
    typedef Target target_type;
    typedef std::shared_ptr<target_type> target_ptr_type;

    abstract_clang_node(const origin_type *origin)
        : origin(origin) {}

    operator bool() const { return origin; }
    const origin_type* operator ->() const { return origin; }
    const origin_type* operator *() const { return origin; }

    virtual target_ptr_type km2_node() const = 0;
};

#endif // ABSTRACT_CLANG_NODE_H
