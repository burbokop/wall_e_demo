#ifndef KM2_ERROR_H
#define KM2_ERROR_H

#include <string>



namespace km2 {

class error {
    std::string m_message;
    std::string::size_type m_start;
    std::string::size_type m_end;
public:

    error();

    error(
            std::string message,
            std::string::size_type start,
            std::string::size_type end
            );
    const std::string &message() const;
    std::string::size_type start() const;
    std::string::size_type end() const;
};

} // namespace km2

bool operator==(const km2::error &error0, const km2::error &error1);


#endif // KM2_ERROR_H
