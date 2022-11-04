#include "jit_spec.h"

#include <iostream>

#include "../../src/km2/km2.h"
#include "../../src/km2/backend/unit/unit.h"
#include <src/km2/backend/entities/function.h>


#ifdef LLVM_ENABLED
#include "../../src/km2/llvm_backend/llvm_backend.h"
#endif
#include "../../src/km2/interpreter_backend/interpreter_backend.h"


static void jit_test(const km2::backend::backend* b) {
    const auto code = "namespace goga {\n"
                      "printf = (format string, args ...) u32;\n"
                      "printf('start\n');\n"
                      "pow = (a f64, b f64) f64;\n"
                      "print = (num f64) {\n"
                      "    printf('gogadoda: %f\n', pow(num, 2.));\n"
                      "};\n"
                      "print(444.);\n"
                      "}\n";

    const auto res = km2::compile(b, code);
    wall_e_should_be_defined(res.unit());
    wall_e_should_be_defined(res.backend_value());
    auto entry = dynamic_cast<km2::backend::function*>(res.backend_value());
    wall_e_should_be_defined(entry);
    wall_e_should_be_right(res.unit()->run_jit(entry));
}

void km2::jit_spec::llvm_jit_test() {
#ifdef LLVM_ENABLED
    km2::llvm_backend::backend b;
    jit_test(&b);
#else
    std::cout << "llvm not enabled. skiping" << std::endl;
#endif
}

void km2::jit_spec::interpreter_jit_test() {
    km2::interpreter_backend::backend b;
    jit_test(&b);
}
