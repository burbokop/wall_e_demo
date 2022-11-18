#include "markup_string.h"

#include <wall_e/src/lex.h>

std::ostream &km2::operator<<(std::ostream &stream, const markup_string &s) {
    return stream << "{ str: " << s.str() << ", format: " << s.format() << " }";
}

static std::string plain_to_md(const std::string& str) {
    return wall_e::lex::replace(str, {
                       { "<", "\\<" },
                       { ">", "\\>" }
                   });
}

km2::markup_string km2::markup_string::convert_to(enum format fmt) const {
    if(m_format == fmt) {
        return *this;
    } else {
        if(m_format == PlainText && fmt == Markdown) {
            return km2::markup_string(plain_to_md(m_data), fmt);
        }
    }
    return *this;
}
