#ifndef BACKENDFACTORY_H
#define BACKENDFACTORY_H

#include <QObject>
#include "klibcore/kmacro.h"

namespace km2 {
namespace backend {
class backend;
}
}

class BackendSharedPtr {
    Q_GADGET
    std::shared_ptr<km2::backend::backend> m_b;
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(bool valid READ valid)

    QString m_reserveName;
    BackendSharedPtr(const QString& name) : m_b(nullptr), m_reserveName(name) {}
public:
    BackendSharedPtr(const std::shared_ptr<km2::backend::backend>& b = nullptr) : m_b(b) {}
    static BackendSharedPtr newEmpty(const QString& name) { return BackendSharedPtr(name); }
    QString name() const;
    inline bool valid() const { return m_b ? true : false; }
    std::shared_ptr<km2::backend::backend> data_ptr() const { return m_b; }
    inline bool operator==(const BackendSharedPtr& other) const { return m_b == other.m_b; }
};


class BackendFactory : public QObject {
    Q_OBJECT
    K_SAVED_PROPERTY(QString, currentBackendName, currentBackendName, setCurrentBackendName, currentBackendNameChanged, {})
    K_READONLY_PROPERTY(BackendSharedPtr, currentBackend, currentBackend, setCurrentBackend, currentBackendChanged, BackendSharedPtr())
    K_READONLY_PROPERTY(QStringList, availableBackends, availableBackends, setAvailableBackends, availableBackendsChanged, {})
    QList<BackendSharedPtr> m_backends;

    void register_backends();
public:
    explicit BackendFactory(QObject *parent = nullptr);

signals:

};

Q_DECLARE_METATYPE(BackendSharedPtr)

#endif // BACKENDFACTORY_H
