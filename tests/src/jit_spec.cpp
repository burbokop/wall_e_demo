#include "jit_spec.h"

#include <iostream>

#include "../../km2/km2.h"
#include "../../km2/translation_unit/translation_unit.h"


#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/TargetSelect.h"


void km2::jit_spec::jit_test0() {
    const auto code = "namespace goga {\n"
                      "printf = (format string, args ...) u32;\n"
                      "printf('start\n');\n"
                      "pow = (a f64, b f64) f64;\n"
                      "print = (num f64) {\n"
                      "    printf('gogadoda: %f\n', pow(num, 2.));\n"
                      "};\n"
                      "print(444.);\n"
                      "}\n";

    const auto res = km2::compile(code);
    wall_e_should_be_defined(res.unit());
    wall_e_should_be_defined(res.llvm_value());
    auto entry = static_cast<llvm::Function*>(res.llvm_value());
    wall_e_should_be_defined(entry);
    wall_e_should_be_right(res.unit()->run_jit(entry));
}
