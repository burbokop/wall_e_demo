#ifndef KM2_H
#define KM2_H

#include <string>
#include <wall_e/src/variant.h>
#include <wall_e/src/lex.h>
#include <wall_e/src/asm_tools.h>

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
