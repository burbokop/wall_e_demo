#ifndef ABSTRACT_CAPABILITY_H
#define ABSTRACT_CAPABILITY_H

#include <functional>
#include <map>
#include "../../as.h"
#include <ostream>
#include <wall_e/src/utility/collections.h>

namespace km2 {
namespace backend {

class unit;

class abstract_capability {
    km2_backend_decl_as
    unit* m_unit = nullptr;
public:
    abstract_capability(unit* unit);
    unit *unit() const;
    virtual ~abstract_capability();

    typedef std::function<abstract_capability*(class unit*)> factory;
    typedef wall_e::map<std::size_t, abstract_capability::factory> factory_map;
};

}
}

#endif // ABSTRACT_CAPABILITY_H
