
#include "treegui/mainwindow.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QResource>
#include <QDirIterator>
#include <QQmlContext>

#include <string.h>
#include <fstream>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <wall_e/src/kgram_smp.h>
#include <src/treegui/appcore.h>

int main(int argc, char **argv) {
    std::string lastArg;

    std::string outputFilePath;
    std::string input;
    bool textInputMode = true;
    bool onlyTree = false;
    for(int i = 0; i < argc; ++i) {
        std::string arg(argv[i]);
        if(lastArg == "-t") {
            textInputMode = true;
            input = arg;
        } else if(lastArg == "-o") {
            outputFilePath = arg;
        } else if(arg == "--tree") {
            onlyTree = true;
        } else if(arg != "-t" && arg != "-o") {
            textInputMode = false;
            input = arg;
        }
        lastArg = arg;
    }

    if(!textInputMode) {
        std::ifstream inpf(input, std::ios::in);
        input.clear();
        while (!inpf.eof()) {
            char c;
            inpf.read(&c, 1);
            input.push_back(c);
        }
        inpf.close();
    }


    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qmlRegisterType<KGramTreeView>("Km2", 1, 0, kgram_type<KGramTreeView>().c_str());
    QCoreApplication::setOrganizationName("bacul14");
    QCoreApplication::setOrganizationDomain("bacul14.kl.ua");

    Q_INIT_RESOURCE(resources);

    QGuiApplication app(argc, argv);
    AppCore appCore(&app);
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appCore", &appCore);
    engine.load("qrc:/resources/main.qml");

    kgram_rule_t::assignTypeSymbol(kgram_rule_type_t::Conjunction, '&');
    kgram_rule_t::assignTypeSymbol(kgram_rule_type_t::Disjunction, '|');

    return app.exec();
}
