#ifndef SIZE_T_PTR_H
#define SIZE_T_PTR_H

#include <cstdint>
#include <limits>

namespace km2 {
namespace interpreter_backend {

typedef std::size_t size_t_ptr;
constexpr size_t_ptr null_size_t_ptr = std::numeric_limits<std::size_t>::max();

}
}

#endif // SIZE_T_PTR_H
