
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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wall_e/src/utility/asm_tools.h>
#include <src/treegui/appcore.h>
#include <src/cmp.h>
#include <wall_e/src/flag.h>

int main(int argc, char **argv) {
    wall_e::flag_provider flag_provider(argc, argv);

    std::cout << flag_provider.value_flag(std::pair { 'i', "input" }, "input", "def_in") << "\n";
    std::cout << flag_provider.value_flag('o', "output", "def_out") << "\n";
    std::cout << flag_provider.bool_flag('f', "flag") << "\n";
    std::cout << flag_provider.bool_flag(std::pair { 'g', "gog" }, "goga") << "\n";
    std::cout << flag_provider.value_flag('a', "A", "AAA") << "\n";

    flag_provider.finish(std::cout);


    wall_e::gram::rule::assignTypeSymbol(wall_e::gram::rule_type::Conjunction, '&');
    wall_e::gram::rule::assignTypeSymbol(wall_e::gram::rule_type::Disjunction, '|');

    const auto r0 = wall_e::gram::rule_from_str("cmd & SEMICOLON & (0 | block)");
    const auto r1 = (wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule() | "block");

    //std::cout << "r0: " << r0 << " : " << smp::simplify(r0) << '\n';
    //std::cout << "r1: " << r1 << " : " << smp::simplify(r1) << '\n';

    return 0;

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
    qmlRegisterType<KGramTreeView>("Km2", 1, 0, wall_e::type_name<KGramTreeView>().c_str());
    QCoreApplication::setOrganizationName("bacul14");
    QCoreApplication::setOrganizationDomain("bacul14.kl.ua");

    Q_INIT_RESOURCE(resources);

    QGuiApplication app(argc, argv);
    AppCore appCore(&app);
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appCore", &appCore);
    engine.load("qrc:/resources/main.qml");



    return app.exec();
}
