#ifndef KM2_LLVM_BACKEND_VALUE_H
#define KM2_LLVM_BACKEND_VALUE_H

#include "../../backend/entities/value.h"
#include <memory>

namespace llvm {
class Value;
}

namespace km2 {
namespace llvm_backend {

class type;

class value : public backend::value {
    llvm::Value* m_data;
public:
    value(llvm::Value* data) : m_data(data) {}
    inline static std::unique_ptr<value> make_uniq(llvm::Value* data) { return std::make_unique<value>(data); }

    llvm::Value* data() const { return m_data; };

    virtual backend::type* value_type() const override;
};

}
}

#endif // KM2_LLVM_BACKEND_VALUE_H
