#include "iofind.h"

#include <string>

void iofind(std::istream &stream, const std::string &str) {
    std::size_t i = 0;
    char c;
    while (i < str.size()) {
        stream >> c;
        if(c == str[i]) {
            ++i;
        } else {
            i = 0;
        }
    }
}
