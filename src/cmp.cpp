#include "cmp.h"



wall_e::gram::rule smp::simplifier::exec(const wall_e::gram::rule &r) {
    if(r.isContainer()) {
        auto children = r.children();
        for(size_t i = 0; i < children.size(); ++i) {
            children[i] = exec(children[i]);

            const auto it = m_operators.find({ r.type(), children[i].type() });
            if(it != m_operators.end()) {
                //r = it->second(r, i);
            }
        }
        for(const auto& child : children) {
            const auto it = m_operators.find({ r.type(), child.type() });
            if(it != m_operators.end()) {
                //it->second(exec(child));
            }
        }
    }
    return r;
}

wall_e::gram::rule smp::simplify(const wall_e::gram::rule &rule) {
    simplifier s;
    //s.installOperator({ rule_type::Conjunction, rule_type::Conjunction }, [](const wall_e::gram::rule& value, size_t i) -> decltype (value) {
    //
    //});
    return s.exec(rule);
}
