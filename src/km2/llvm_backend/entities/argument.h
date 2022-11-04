#ifndef KM2_LLVM_BACKEND_ARGUMENT_H
#define KM2_LLVM_BACKEND_ARGUMENT_H

#include "../../backend/entities/argument.h"

namespace llvm {
class Argument;
}

namespace km2 {
namespace llvm_backend {

class argument : public backend::argument {
    llvm::Argument *m_data;
public:
    argument(llvm::Argument *data) : m_data(data) {};

    llvm::Argument *data() const { return m_data; };

    // value interface
public:
    virtual backend::type *value_type() const override;
};

}
}

#endif // KM2_LLVM_BACKEND_ARGUMENT_H
