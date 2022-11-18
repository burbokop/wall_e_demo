#include "textedithover.h"

#include <QTimer>


QTimer *TextEditHover::timer() const {
    if(!m_timer) m_timer = new QTimer(const_cast<TextEditHover*>(this));
    return m_timer;
}

void TextEditHover::sync(QQuickTextEdit *t) {
    if(m_prev_t) {
        m_prev_t->removeEventFilter(this);
    }
    if(t) {
        t->installEventFilter(this);
    }
    m_prev_t = t;
}

TextEditHover::TextEditHover(QObject *parent)
    : QObject { parent } {
    connect(this, &TextEditHover::textEditChanged, this, &TextEditHover::sync);
    sync(textEdit());
    connect(timer(), &QTimer::timeout, this, [this](){
        emit hover(m_pos);
        timer()->stop();
    });
}

bool TextEditHover::eventFilter(QObject *watched, QEvent *event) {
    if(const auto& hoverEvent = dynamic_cast<QHoverEvent*>(event)) {
        if(const auto& te = qobject_cast<QQuickTextEdit*>(watched)) {
            const auto& pos = hoverEvent->position();
            m_pos = te->positionAt(pos.x(), pos.y());
            if(delay() == 0) {
                emit hover(m_pos);
            } else {
                timer()->stop();
                timer()->start(delay());
            }
            return true;
        }
    }
    return false;
}
