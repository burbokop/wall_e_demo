#include "function.h"


std::string km2::function::produce_name(const std::string &name, const std::vector<std::string> &typenames) {
    std::string result = name;
    for(auto t : typenames) {
        result += "_" + t;
    }
    return result;
}

std::vector<std::string> km2::function::argTypes() const {
    return m_argTypes;
}

km2::function km2::function::choose_overload(const std::list<function> &overloads, const wall_e::variant_vector &args, const std::function<bool (const std::string&, const wall_e::variant&)> &matchRule) {
    for(auto o : overloads) {
        const auto types = o.argTypes();
        if(types.size() == args.size()) {
            for(size_t i = 0; i < types.size(); ++i) {
                if(matchRule(types[i], args[i])) {
                    return o;
                }
            }
        }
    }
    return function();
}

std::list<km2::function> km2::function::find_overloads(const std::string &name, const std::list<function> &functions) {
    std::list<function> result;
    for(auto f : functions) {
        if(f.originalName() == name) {
            result.push_back(f);
        }
    }
    return result;
}

std::string km2::function::fullName() const {
    return m_fullName;
}

std::string km2::function::originalName() const {
    return m_originalName;
}

km2::function::function(const std::string &name, const std::vector<std::string> &typenames) {
    m_originalName = name;
    m_argTypes = typenames;
    m_fullName = produce_name(name, typenames);
}

km2::function::function(const std::string &name, const std::vector<std::pair<wall_e::lex::token, wall_e::lex::token> > &typenames) {
    std::vector<std::string> tn;
    tn.resize(typenames.size());
    for(size_t i = 0, count = typenames.size(); i < count; ++i) {
        tn[i] = typenames[i].first.name;
    }

    m_originalName = name;
    m_argTypes = tn;
    m_fullName = produce_name(name, tn);
}

std::ostream &km2::operator<<(std::ostream &stream, const km2::function &function) {
    stream << function.fullName();
    return stream;
}

bool km2::operator==(const km2::function &function0, const km2::function &function1) {
    return function0.fullName() == function1.fullName();
}
