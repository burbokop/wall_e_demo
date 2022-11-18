#ifndef KM2_MARKUP_STRING_H
#define KM2_MARKUP_STRING_H

#include <string>
#include <ostream>

namespace km2 {

struct markup_string {
    enum format {
        PlainText = 0,
        Markdown
    };
private:
    std::string m_data;
    format m_format;
public:
    markup_string()
        : m_data({}),
          m_format(PlainText) {}
    markup_string(const std::string& data, format format)
        : m_data(data),
          m_format(format) {}

    const std::string& str() const { return m_data; }
    format format() const { return m_format; }

    inline markup_string operator+(const std::string& other) const {
        return markup_string(m_data + other, m_format);
    }

    inline markup_string operator+(const markup_string& other) const {
        const auto& c = other.convert_to(m_format);
        return markup_string(m_data + c.m_data, m_format);
    }

    markup_string convert_to(enum format fmt) const;

    bool operator==(const markup_string& other) const {
        return m_data == other.m_data && m_format == other.m_format;
    }
};

std::ostream& operator<<(std::ostream& stream, const markup_string& s);

namespace literals {
inline markup_string operator "" _plain(const char* c, std::size_t s) {
    return markup_string(std::string(c, c + s), markup_string::PlainText);
}
inline markup_string operator "" _md(const char* c, std::size_t s) {
    return markup_string(std::string(c, c + s), markup_string::Markdown);
}
}

}

#endif // MARKUP_STRING_H
