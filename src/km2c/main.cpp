

#include <wall_e/src/flag.h>
#include "../km2/km2.h"
#include "wall_e/src/utility/tree_view_tools.h"
#include <fstream>
#include <src/km2/backend/entities/function.h>
#include <src/km2/backend/unit/unit.h>


#include <src/km2/interpreter_backend/interpreter_backend.h>

int main(int argc, const char** argv) {
    wall_e::map<std::string, std::function<km2::backend::backend*()>> backends = {
        { "interpreter", []{ return new km2::interpreter_backend::backend(); } }
    };
    const std::string default_backend_name = "interpreter";

    wall_e::flag_provider flags(argc, argv);
    std::cout << "flags_0: " << flags << std::endl;
    const auto input_flag = flags.value_flag(std::pair { 'c', "" }, "compile");
    const auto output_flag = flags.value_flag('o', "output", "a.km2.exe");
    std::cout << flags.bool_flag('f', "flag") << std::endl;
    const auto backend_name_flag = flags.value_flag(std::pair { 'B', "backend" }, "beckend", default_backend_name);
    const auto verbose_flag = flags.bool_flag('v', "verbose");
    const auto tree_flag = flags.bool_flag('t', "tree");
    const auto jit_flag = flags.bool_flag('j', "jit");

    std::cout << "flags_1: " << flags << std::endl;
    flags.finish(std::cout);

    km2::flags compile_flags;
    if(verbose_flag.bool_data()) {
        compile_flags.push_back(km2::Verbose);
    }
    if(tree_flag.bool_data()) {
        compile_flags.push_back(km2::OnlyTree);
    }

    if(const auto& backend = backends.find_opt(backend_name_flag.data())) {
        const auto result = km2::compile((*backend)(), std::ifstream(input_flag.data()), input_flag.data(), compile_flags);

        if(result.errors().size() > 0) {
            std::cerr << "compile errors: " << result.errors() << std::endl;
        }

        if(tree_flag.bool_data()) {
            wall_e::write_tree(result.token_tree());
        } else if(jit_flag.bool_data()) {
            if(result.unit()) {
                if(const auto& entry = dynamic_cast<km2::backend::function*>(result.backend_value())) {
                    if(const auto& res = result.unit()->run_jit(entry, verbose_flag.bool_data())) {
                        return res;
                    } else {
                        std::cerr << "jit error: " << res.left_value() << std::endl;
                    }
                } else {
                    std::cerr << "can not jit: entry is not a function (entry: " << result.backend_value() << ")" << std::endl;
                }
            } else {
                std::cerr << "can not jit: unit not exit" << std::endl;
            }
        } else {
            if(result.unit()) {
                if(const auto& cres = result.unit()->compile(output_flag.data())) {
                    std::cerr << "compilation compleated with code: " << cres.right_value() << std::endl;
                } else {
                    std::cerr << "err compiling unit: " << cres.left_value() << std::endl;
                }
            } else {
                std::cerr << "err: unit not exit" << std::endl;
            }
        }
    } else {
        std::cerr << "backend " << backend_name_flag.data() << "not found" << std::endl;
        std::cout << "available backedns: " << backends.keys() << std::endl;
    }
}
