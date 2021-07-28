#ifndef ABSTRACTTHREAD_H
#define ABSTRACTTHREAD_H

#include <QThread>

class AbstractThread : public QThread {
    Q_OBJECT
    std::function<int()> m_func;
public:
    explicit AbstractThread(QObject *parent = nullptr);

    void start(const std::function<int()> &func);

    // QThread interface
protected:
    virtual void run() override;


signals:
    void resultReady(int);
};

#endif // ABSTRACTTHREAD_H
