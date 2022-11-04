#include "interpreter_backend.h"

#include <wall_e/src/utility/typename.h>

#include "unit/unit.h"
#include "unit/capabilities/constants_capability.h"
#include "unit/capabilities/namespace_capability.h"
#include "unit/capabilities/type_capability.h"
#include "entities/block.h"
#include "entities/function.h"

std::string km2::interpreter_backend::backend::name() const {
    return wall_e::type_name<std::remove_pointer<decltype(this)>::type>();
}

km2::backend::abstract_capability::factory_map km2::interpreter_backend::backend::register_unit_caps() const {
    return {
        {
            typeid(km2::backend::constants_capability).hash_code(),
                    [](km2::backend::unit* u)
            {
                return new km2::interpreter_backend::constants_capability(u);
            }
        }, {
            typeid(km2::backend::namespace_capability).hash_code(),
                    [](km2::backend::unit* u)
            {
                return new km2::interpreter_backend::namespace_capability(u);
            }
        }, {
            typeid(km2::backend::type_capability).hash_code(),
                    [](km2::backend::unit* u)
            {
                return new km2::interpreter_backend::type_capability(u);
            }
        }
    };
}

km2::backend::unit::factory km2::interpreter_backend::backend::register_unit_factory() const {
    return [](const km2::backend::backend* b) -> std::shared_ptr<km2::backend::unit> {
        return std::make_shared<km2::interpreter_backend::unit>(b);
    };
}
