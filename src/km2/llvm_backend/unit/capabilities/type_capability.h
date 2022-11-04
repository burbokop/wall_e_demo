#ifndef KM2_LLVM_BACKEND_TYPE_CAPABILITY_H
#define KM2_LLVM_BACKEND_TYPE_CAPABILITY_H

#include "../../../backend/unit/capabilities/type_capability.h"
#include <wall_e/src/utility/collections.h>

namespace llvm {
class Type;
}

namespace km2 {
namespace llvm_backend {

class type;

class type_capability : public backend::type_capability {
    wall_e::list<std::unique_ptr<type>> m_types;
    llvm_backend::type* born_type(llvm::Type* data);
public:
    type_capability(backend::unit* unit);

    virtual backend::type* int32() override;
    virtual backend::type* float32() override;
    virtual backend::type* float64() override;
    virtual backend::type* custom_int(std::size_t bits) override;
    virtual backend::type* void_type() override;
    virtual backend::type* cstr() override;
    virtual backend::type* func_type(backend::type* return_type, wall_e::vec<backend::type*> arg_types) override;
};

}
}
#endif // TYPE_CAPABILITY_H
