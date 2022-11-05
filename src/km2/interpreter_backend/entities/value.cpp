#include "value.h"

#include "argument.h"
#include "type.h"
#include "../instructions/instruction.h"

km2::interpreter_backend::value *km2::interpreter_backend::value::downcast(km2::backend::value *t) {
    if(auto v = dynamic_cast<value*>(t)) {
        return v;
    } else if(auto v = dynamic_cast<argument*>(t)) {
        return v;
    }
    return nullptr;
}

km2::backend::type *km2::interpreter_backend::value::value_type() const {
    return m_type;
}

