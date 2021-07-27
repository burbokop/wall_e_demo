#ifndef ABSTRACT_NODE_H
#define ABSTRACT_NODE_H

#include <src/km2/error.h>
#include <list>

namespace km2 {

class abstract_node {
    wall_e::text_segment m_segment;
public:
    abstract_node(const wall_e::text_segment& segment = {});
    wall_e::text_segment segment() const;

    virtual void print(size_t level, std::ostream &stream) = 0;
    virtual std::list<km2::error> errors() = 0;
};

} // namespace km2

#endif // ABSTRACT_NODE_H
