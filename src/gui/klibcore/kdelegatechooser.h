#ifndef KDELEGATECHOOSER_H
#define KDELEGATECHOOSER_H

#include "kdelegate.h"

#include <QQuickItem>
#include "kmacro.h"

/**
 * @brief The KDelegateChooser class provides making lists in qml with different viwes for different types.
 *
 * @details Example:
 *
 *
 * QObject class on c++
 * ```
 *  #include <klibcore/src/kmacro.h>
 *
 *  class A : public QObject { Q_OBJECT }
 *  class B : public QObject { Q_OBJECT }
 *
 *  AppCore : public QObject {
 *      Q_OBJECT
 *      K_QML_TYPE(AppCore)
 *      QList<QObject*> m_list = { new A(), new B(), new A() }
 *      K_READONLY_LIST_PROPERTY(QObject, list, m_list)
 *  }
 * ```
 * Code in qml
 * ```
 *  ListView {
 *      AppCore {
 *          id: appCore
 *      }
 *
 *      model: appCore.list
 *
 *      delegate: KDelegateChooser {
 *          delegates: [
 *              KDelegate {
 *                  modelDataType: "A"
 *                  Rectangle { color: "#ff0000" }
 *              }
 *              KDelegate {
 *                  modelDataType: "B"
 *                  Rectangle { color: "#00ff00" }
 *              }
 *          ]
 *      }
 *  }
 *
 * ```
 *
 *
 * @note This class is available only from QML
 */
class KDelegateChooser : public QQuickItem {
    Q_OBJECT
    K_QML_TYPE(KDelegateChooser)

    QList<KDelegate *> m_delegates;
    K_LIST_PROPERTY(KDelegate, delegates, m_delegates)
    K_READONLY_PROPERTY(QObject*, currentDelegate, currentDelegate, setCurrentDelegate, currentDelegateChanged, nullptr)
public:
    explicit KDelegateChooser(QQuickItem *parent = nullptr);

signals:


};

#endif // KDELEGATECHOOSER_H
