#ifndef PRINTABLE_H
#define PRINTABLE_H

#include <ostream>


//#include <wall_e/src/utility/collections.h>

namespace km2 {
namespace backend {

class printable {
public:
    printable() {}
    virtual std::ostream& print(std::ostream&) const = 0;
    virtual ~printable() {}
};

inline std::ostream& operator<<(std::ostream& stream, const printable* p) { return p->print(stream); }

}
}

#endif // PRINTABLE_H
