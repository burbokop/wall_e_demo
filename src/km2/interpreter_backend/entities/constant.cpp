#include "constant.h"
#include "wall_e/src/utility/typename.h"

#include "type.h"
#include "../instructions/call_instruction.h"

std::ostream &km2::interpreter_backend::constant::print(std::ostream &stream) const {
    return stream << wall_e::type_name<constant>() << " "
                  << "{ type: " << value_type()
                  << ", dyn: " << m_dyn
                  << " }";
}
