#ifndef TEXTEDITHOVER_H
#define TEXTEDITHOVER_H

#include <QObject>
#include "../klibcore/kmacro.h"

//QT_BEGIN_NAMESPACE
//class QQuickTextEdit;
//QT_END_NAMESPACE
#include <private/qquicktextedit_p.h>

class TextEditHover : public QObject {
    Q_OBJECT
    K_AUTO_PROPERTY(QQuickTextEdit*, textEdit, textEdit, setTextEdit, textEditChanged, nullptr)
    K_AUTO_PROPERTY(int, delay, delay, setDelay, delayChanged, 1000)

    QQuickTextEdit* m_prev_t = nullptr;
    mutable QTimer* m_timer = nullptr;
    QTimer* timer() const;

    int m_pos = 0;

    void sync(QQuickTextEdit* t);
public:
    explicit TextEditHover(QObject *parent = nullptr);

    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void hover(int pos);
};

#endif // TEXTEDITHOVER_H
