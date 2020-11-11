#include "kdelegatechooser.h"
#include <QTimer>

#include <QQmlContext>

KDelegateChooser::KDelegateChooser(QQuickItem *parent) : QQuickItem(parent) {
    QMetaObject::invokeMethod(this, [this](){
        QString type;
        auto modelData = qmlContext(this)->contextProperty("modelData");
        if(auto object = modelData.value<QObject*>()) {
            type = object->metaObject()->className();
        } else {
            type = modelData.typeName();
        }

        for(auto d : m_delegates) {
            if(d->modelDataType() == type) {
                if(currentDelegate())
                    currentDelegate()->deleteLater();

                setCurrentDelegate(d->contentComponent()->create(qmlContext(this)));
                if(currentDelegate()) {
                    auto currentDelegateItem = qobject_cast<QQuickItem*>(currentDelegate());
                    if(currentDelegateItem) {
                        currentDelegateItem->setParentItem(this);
                    } else {
                        currentDelegate()->setParent(this);
                    }
                }
                break;
            }
        }
    }, Qt::QueuedConnection);
}

