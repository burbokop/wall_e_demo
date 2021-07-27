#ifndef KM2_ERROR_H
#define KM2_ERROR_H

#include "text_segment.h"

namespace km2 {

class error {
    std::string m_message;
    text_segment m_segment;
public:
    error();
    error(const std::string& message);
    error(
            const std::string& message,
            const text_segment& segment
            );

    std::string message() const;
    text_segment segment() const;
};

} // namespace km2

bool operator==(const km2::error &error0, const km2::error &error1);

std::ostream& operator<<(std::ostream& stream, const km2::error &error);


#endif // KM2_ERROR_H
