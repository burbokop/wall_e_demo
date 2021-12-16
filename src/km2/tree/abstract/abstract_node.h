#ifndef ABSTRACT_NODE_H
#define ABSTRACT_NODE_H

#include <src/km2/error.h>
#include <list>
#include <functional>
#include <wall_e/src/private/gram_private.h>

namespace km2 {

class abstract_node {
    wall_e::text_segment m_segment;
public:
    typedef std::function<wall_e::gram::argument(const wall_e::gram::arg_vector &)> factory;

    abstract_node(const wall_e::text_segment& segment = {});
    wall_e::text_segment segment() const;

    virtual void print(size_t level, std::ostream &stream) = 0;
    virtual std::list<km2::error> errors() = 0;
};

} // namespace km2

#endif // ABSTRACT_NODE_H
