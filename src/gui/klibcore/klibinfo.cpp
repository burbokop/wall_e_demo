#include "klibinfo.h"

#include <QQmlEngine>

const int KLibInfo::static_call = [](){
    qmlRegisterModule("KLib", 1, 0);
    return 0;
}();
