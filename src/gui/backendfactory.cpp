#include "backendfactory.h"

#ifdef LLVM_ENABLED
#include "../../src/km2/llvm_backend/llvm_backend.h"
#endif
#include "../../src/km2/interpreter_backend/interpreter_backend.h"

const BackendSharedPtr emptyBackend = BackendSharedPtr::newEmpty("no backend");

BackendFactory::BackendFactory(QObject *parent)
    : QObject{parent} {
    register_backends();
    connect(this, &BackendFactory::currentBackendNameChanged, this, [this](const QString& name) {
        for(const auto& b : m_backends) {
            if(b.name() == name) {
                setCurrentBackend(b);
                return;
            }
        }
        setCurrentBackend(emptyBackend);
    });
    QMetaObject::invokeMethod(this, [this](){
        emit currentBackendNameChanged(currentBackendName());
    });
}

void BackendFactory::register_backends() {
    m_backends = {
    #ifdef LLVM_ENABLED
        BackendSharedPtr(std::make_shared<km2::llvm_backend::backend>()),
    #endif
        BackendSharedPtr(std::make_shared<km2::interpreter_backend::backend>()),
        emptyBackend
    };


    QStringList l;
    for(const auto& b : m_backends) { l.push_back(b.name()); }
    setAvailableBackends(l);
}

QString BackendSharedPtr::name() const {
    return m_b ? QString::fromStdString(m_b->name()) : m_reserveName;
}
