#include "abstractthread.h"

AbstractThread::AbstractThread(QObject *parent) : QThread(parent) {
}

void AbstractThread::start(const std::function<int ()> &func) {
    m_func = func;
    QThread::start();



}

void AbstractThread::run() {
    emit resultReady(m_func());
}
