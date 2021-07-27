#include "error.h"

#include <ostream>


std::string km2::error::message() const { return m_message; }

wall_e::text_segment km2::error::segment() const { return m_segment; }

km2::error::error() {}

km2::error::error(const std::string &message) { m_message = message; }

km2::error::error(const std::string &message, const wall_e::text_segment &segment) {
    m_message = message;
    m_segment = segment;
}

bool operator==(const km2::error &error0, const km2::error &error1) {
    return error0.message() == error1.message() && error0.segment() == error1.segment();
}

std::ostream &operator<<(std::ostream &stream, const km2::error &error) {
    return stream << error.message() << " " << error.segment();
}

