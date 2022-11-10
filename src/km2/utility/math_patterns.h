#ifndef MATH_PATTERNS_H
#define MATH_PATTERNS_H

#include <list>
#include <string>
#include "wall_e/src/gram.h"

namespace wall_e {
namespace gram { class pattern; }
}

namespace km2 {

class math_patterns {
public:
    static wall_e::gram::rule add_to(std::list<wall_e::gram::pattern> *patterns, const std::string &preffix);
    static wall_e::gram::argument add_sub_processor(const wall_e::gram::arg_vector &args, const wall_e::index &index);
    static wall_e::gram::argument mul_div_processor(const wall_e::gram::arg_vector &args, const wall_e::index &index);
    static wall_e::gram::argument binary_int_operator(const wall_e::gram::arg_vector &args, std::list<std::pair<std::string, std::function<int(int, int)>>> l);

    static bool extract_number_str(const std::string &arg, int *number);
    static bool extract_number(const wall_e::gram::argument &arg, int *number);
};

}
#endif // MATH_PATTERNS_H
