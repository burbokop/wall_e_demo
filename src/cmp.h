#ifndef CMP_H
#define CMP_H

#include <wall_e/src/private/gram_private.h>
#include <map>

namespace smp {
typedef wall_e::gram::rule_type::enum_t rule_type;
typedef std::pair<rule_type, rule_type> rule_type_pair;
typedef std::function<wall_e::gram::rule(const wall_e::gram::rule&, size_t)> oper;

class simplifier {
    std::map<rule_type_pair, oper> m_operators;
public:
    void installOperator(const rule_type_pair& key, const oper& o) { m_operators[key] = o; }
    simplifier() {}
    wall_e::gram::rule exec(const wall_e::gram::rule &r);
};

wall_e::gram::rule simplify(const wall_e::gram::rule &rule);
}
#endif // CMP_H
