#ifndef IOFIND_H
#define IOFIND_H

#include <istream>

void iofind(std::istream& stream, const std::string& str);

#include <variant>

inline std::ostream& operator<<(std::ostream& os, const std::monostate& s) {
    os << "nonostate";
    return os;
}

template<typename T>
struct streamer {
    const T& val;
};
template<typename T> streamer(T) -> streamer<T>;

template<typename T>
std::ostream& operator<<(std::ostream& os, streamer<T> s) {
    os << s.val;
    return os;
}

template<typename... Ts>
std::ostream& operator<<(std::ostream& os, streamer<std::variant<Ts...>> sv) {
    std::visit([&os](const auto& v) { os << streamer{v}; }, sv.val);
    return os;
}



#endif // IOFIND_H
