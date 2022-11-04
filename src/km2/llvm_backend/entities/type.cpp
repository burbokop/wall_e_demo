#include "type.h"

#include <llvm/IR/Type.h>


bool km2::llvm_backend::type::eq(const backend::type *other) const {
    return m_data->getTypeID() == dynamic_cast<const type*>(other)->m_data->getTypeID();
}
