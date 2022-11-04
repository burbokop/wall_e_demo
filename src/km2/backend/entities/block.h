#ifndef KM2_BACKEND_BLOCK_H
#define KM2_BACKEND_BLOCK_H

#include "../as.h"
#include "printable.h"
#include <string>

namespace km2 {
namespace backend {

class block : public printable {
    km2_backend_decl_as
public:
    block() {}
    virtual std::string name() const = 0;
};

}
}

#endif // KM2_BACKEND_BLOCK_H
