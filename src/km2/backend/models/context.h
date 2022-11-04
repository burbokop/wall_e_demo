#ifndef CONTEXT_H
#define CONTEXT_H

#include "overload.h"
#include <wall_e/src/utility/collections.h>

namespace km2 {
namespace backend {

class function_ref;

class context {
    wall_e::list<std::shared_ptr<overload>> m_overloads;
public:
    context();
    std::list<std::shared_ptr<overload>> overloads() const;

    std::optional<wall_e::error> add_overload(const std::shared_ptr<overload>& overload, const wall_e::text_segment& segment);
    std::optional<wall_e::error> add_overload(const function_ref& func, const wall_e::text_segment& segment);

    std::shared_ptr<overload> find_overload(
            const std::list<std::string> &namespace_stack,
            const std::string &name
            );

    template<template<typename, typename> typename C>
    static context sum(const C<context, std::allocator<context>>& container) {
        context acc;
        for(const auto& c : container) {
            acc += c;
        }
        return acc;
    }

    context operator +(const context &other) const;
    inline context operator +=(const context &other) {
        *this = *this + other;
        return *this;
    }

    std::ostream& print(std::ostream& stream) const;
    inline friend std::ostream& operator<<(std::ostream& stream, const context& ctx) { return ctx.print(stream); }
};

}
}


#endif // CONTEXT_H
