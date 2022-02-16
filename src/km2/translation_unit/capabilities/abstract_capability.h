#ifndef ABSTRACT_CAPABILITY_H
#define ABSTRACT_CAPABILITY_H

namespace km2 {

class translation_unit;

class abstract_capability {
    translation_unit* m_unit = nullptr;
public:
    abstract_capability(translation_unit* unit);
    translation_unit *unit() const;
    virtual ~abstract_capability();
};

}
#endif // ABSTRACT_CAPABILITY_H
