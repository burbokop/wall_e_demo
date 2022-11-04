#ifndef ENUMS_SPEC_H
#define ENUMS_SPEC_H

#include "../../wall_e/src/testing.h"

namespace km2 {

class jit_spec {
    static void llvm_jit_test() wall_e_test(jit_spec, llvm_jit_test);
    static void interpreter_jit_test() wall_e_test(jit_spec, interpreter_jit_test);
};

}

#endif // ENUMS_SPEC_H
