#include "argument.h"

#include "type.h"
#include "function.h"
#include "../instructions/instruction.h"
#include "../instructions/dyn_value.h"
#include "block.h"

km2::backend::function *km2::interpreter_backend::argument::func() const {
    return m_func;
}

std::size_t km2::interpreter_backend::argument::index() const {
    return m_index;
}

std::ostream &km2::interpreter_backend::argument::print(std::ostream &stream) const {
    return stream << wall_e::type_name<argument>() << " "
                  << "{ type: " << value_type()
                  << ", dyn: " << dyn()
                  << " }";
}

std::shared_ptr<km2::interpreter_backend::dyn_value> km2::interpreter_backend::argument::dyn() const {
    if(m_func->downcasted_blocks().size() > 0) {
        return dyn_value::from_stack_ptr(m_func->downcasted_blocks().front()->stack_offset() + m_index);
    } else {
        return  nullptr;
    }
}
