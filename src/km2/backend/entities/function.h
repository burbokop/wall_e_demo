#ifndef KM2_BACKEND_FUNCTION_H
#define KM2_BACKEND_FUNCTION_H

#include <string>
#include <list>
#include "value.h"
#include <wall_e/src/utility/collections.h>

namespace km2 {
namespace backend {

class argument;
class block;

class function : public value {
public:
    virtual bool is_var_arg() const = 0;
    virtual wall_e::vec<argument*> args() const = 0;
    virtual type* return_type() = 0;
    virtual std::string name() const = 0;
    virtual wall_e::list<block*> blocks() = 0;
};

}
}
#endif // KM2_BACKEND_FUNCTION_H
