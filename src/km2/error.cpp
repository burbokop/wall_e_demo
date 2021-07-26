#include "error.h"


const std::string &km2::error::message() const {
    return m_message;
}

std::string::size_type km2::error::start() const {
    return m_start;
}

std::string::size_type km2::error::end() const {
    return m_end;
}

km2::error::error() {}

km2::error::error(std::string message, std::string::size_type start, std::string::size_type end) {
    m_message = message;
    m_start = start;
    m_end = end;
}

bool operator==(const km2::error &error0, const km2::error &error1) {
    return error0.message() == error1.message() && error0.start() == error1.start() && error0.end() == error1.end();
}
