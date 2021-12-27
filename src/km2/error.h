#ifndef KM2_ERROR_H
#define KM2_ERROR_H

#if __has_include(<wall_e/src/text_segment.h>)
    #include <wall_e/src/text_segment.h>
#else
    #include <wall_e/text_segment.h>
#endif

namespace km2 {

class error {
    std::string m_message;
    wall_e::text_segment m_segment;
public:
    error();
    error(const std::string& message);
    error(
            const std::string& message,
            const wall_e::text_segment& segment
            );

    std::string message() const;
    wall_e::text_segment segment() const;

};
bool operator==(const km2::error &error0, const km2::error &error1);



} // namespace km2


std::ostream& operator<<(std::ostream& stream, const km2::error &error);


#endif // KM2_ERROR_H
