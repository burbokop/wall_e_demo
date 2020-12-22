#include "to_pdf.h"

#include <fstream>
#include <iostream>
#include <sproc/src/environment.h>

void to_pdf(const wall_e::relation_list &rl, const std::string &path) {
    const auto compile_line = "dot -Tpdf " + path + ".gv -o " + path + ".pdf";
    std::ofstream stream(path + ".gv", std::ios::out);
    stream << "digraph G {\n";
    for(const auto& relation : rl) {
        stream << "\tv" << relation.vertex1 << " -> v" << relation.vertex0;
        if(relation.symbol != 0)
            stream << "[label=\"" << relation.symbol << "\"]";

        stream << ";\n";
    }
    stream << "}\n";
    stream.close();
    std::cout << sproc::home.auto_system(compile_line);
}
