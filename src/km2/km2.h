#ifndef KM2_H
#define KM2_H

#include <string>
#include <wall_e/src/variant.h>
#include <wall_e/src/lex.h>
#include <wall_e/src/utility/asm_tools.h>

template<typename T>
wall_e::gram::argument binary_operator(const wall_e::gram::arg_vector &args, const std::list<std::pair<std::string, std::function<T (T, T)>>> &processors) {
    if(args.size() > 2) {
        const auto val = [](const wall_e::gram::argument &arg, T& value) -> bool {
            if(arg.contains_type<T>()) {
                value = arg.value<T>();
                return true;
            } else if(arg.contains_type<wall_e::lex::token>()) {
                value = arg.value<wall_e::lex::token>().text;
                return true;
            }
            return false;
        };

        T val0, val1;
        if(val(args[0], val0) && args[1].contains_type<wall_e::lex::token>() && val(args[2], val1)) {
            const auto tok = args[1].value<wall_e::lex::token>().name;
            for(const auto processor : processors) {
                if(processor.first == tok) {
                    return processor.second(val0, val1);
                }
            }
        }
    }
    return wall_e::gram::pattern::default_processor(args);
}

template<typename T>
wall_e::gram::argument binary_operator(const wall_e::gram::arg_vector &args, const std::function<T (T, T)> &processor) {
    if(args.size() > 2) {
        const auto val = [](const wall_e::gram::argument &arg, T& value) -> bool {
            if(arg.contains_type<T>()) {
                value = arg.value<T>();
                return true;
            } else if(arg.contains_type<wall_e::lex::token>()) {
                value = arg.value<wall_e::lex::token>().text;
                return true;
            }
            return false;
        };

        T val0, val1;
        if(val(args[0], val0) && val(args[2], val1)) {
            return processor(val0, val1);
        }
    }
    return wall_e::gram::pattern::default_processor(args);
}

template<typename T>
std::pair<T, T> binary_operator(const wall_e::gram::arg_vector &args) {
    if(args.size() > 2) {
        const auto val = [](const wall_e::gram::argument &arg, T& value) -> bool {
            if(arg.contains_type<T>()) {
                value = arg.value<T>();
                return true;
            } else if(arg.contains_type<wall_e::lex::token>()) {
                value = arg.value<wall_e::lex::token>().text;
                return true;
            }
            return false;
        };

        T val0, val1;
        if(val(args[0], val0) && val(args[2], val1)) {
            return { val0, val1 };
        }
    }
    return {};
}



wall_e::gram::rule from_str(const std::string &string);



struct km2_error {
    std::string message;
    std::string::size_type start;
    std::string::size_type end;
};

bool operator==(const km2_error &error0, const km2_error &error1);

struct km2_compilation_result {
    wall_e::variant tree;
    std::vector<wall_e::lex::token> tokens;
    std::string rules;
    wall_e::asm_unit assembly;
    std::list<km2_error> errors;
};

enum km2_flag {
    km2_only_tree,
    km2_verbose
};

typedef std::list<km2_flag> km2_flags;

template <typename T>
std::ostream &operator<<(std::ostream& stream, const std::list<T> &list);


std::string km2_trim_str(const std::string& str, char symbol = ' ');

km2_compilation_result km2_compile(const std::string &input, const km2_flags &flags = km2_flags());

template<typename T>
std::ostream &operator<<(std::ostream &stream, const std::list<T> &list) {
    int i = 0;
    stream << "{ ";
    for(auto l : list) {
        stream << l;
        if(i != list.size() - 1) {
            stream << ",";
        }
        ++i;
    }
    stream << " }";
    return stream;
}

#endif // TEST2_H
