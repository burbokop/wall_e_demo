#include "markup_string.h"

std::ostream &km2::operator<<(std::ostream &stream, const markup_string &s) {
    return stream << "{ str: " << s.str() << ", format: " << s.format() << " }";
}
