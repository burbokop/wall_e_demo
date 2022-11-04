#ifndef KM2_LLVM_BACKEND_TYPE_H
#define KM2_LLVM_BACKEND_TYPE_H

#include "../../backend/entities/type.h"
#include <memory>
namespace llvm { class Type; }

namespace km2 {
namespace llvm_backend {

class type : public backend::type {
    llvm::Type* m_data;
public:
    type(llvm::Type* data) : m_data(data) {}
    inline static std::unique_ptr<type> make_uniq(llvm::Type* data) { return std::make_unique<type>(data); }

    llvm::Type* data() const { return m_data; }


    // type interface
public:
    virtual bool eq(const backend::type *other) const override;
};

}
}

#endif // TYPE_H
