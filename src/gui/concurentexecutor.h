#ifndef CONCURENTEXECUTOR_H
#define CONCURENTEXECUTOR_H

#include <QObject>


class ConcurentExecutor : public QObject {
    Q_OBJECT

public:
    explicit ConcurentExecutor(QObject *parent = nullptr);

    template<typename T>
    void execute(const std::function<T()>& func);

signals:

};


namespace conc_exec {

    void execute(const std::function<void()>& proceed, const std::function<void()>& ready);
};



#endif // CONCURENTEXECUTOR_H
