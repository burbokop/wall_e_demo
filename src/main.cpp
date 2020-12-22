#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <src/treegui/appcore.h>
#include <src/treegui/kgramtreeview.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <QQmlContext>
#include <wall_e/src/flag.h>
#include <wall_e/src/color.h>
#include <src/to_pdf.h>
#include <sproc/src/apt.h>


#include <sproc/src/environment.h>
#include <fstream>

void auto_system_example(const std::list<std::pair<int, int>> &rl, const std::string &path) {
    const auto compile_line = "dot -Tpdf " + path + ".gv -o " + path + ".pdf";
    std::ofstream stream(path + ".gv", std::ios::out);
    stream << "digraph G {\n";
    for(const auto& relation : rl) {
        stream
                << "\tv"
                << relation.second
                << " -> v"
                << relation.first
                << ";\n";
    }
    stream << "}\n";
    stream.close();
    std::cout << sproc::home.auto_system(compile_line);
}


int main(int argc, char **argv) {

    auto_system_example({
                            { 0, 1 },
                            { 0, 2 },
                            { 0, 3 },
                            { 2, 3 }
                        }, "result_file");
    return 0;


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


    //wall_e::write_graph(std::cout, r0.to_graph());

    std::cout << r0 << "\n";
    wall_e::write_graph(std::cout, r0.to_graph());
    std::cout << "sm: " << wall_e::gram::simplify_rule_default(r0) << "\n";
    wall_e::write_graph(std::cout, wall_e::gram::simplify_rule_default(r0).to_graph());

    std::cout << r1 << "\n";
    wall_e::write_graph(std::cout, r1.to_graph());
    std::cout << "sm: " << wall_e::gram::simplify_rule_default(r1) << "\n";
    wall_e::write_graph(std::cout, wall_e::gram::simplify_rule_default(r1).to_graph());

    std::filesystem::create_directory("app_out");
    to_pdf(r0.to_relation_list(), "./app_out/r0");
    to_pdf(r1.to_relation_list(), "./app_out/r1");
    to_pdf(wall_e::gram::simplify_rule_default(r0).to_relation_list(), "./app_out/r0_s");
    to_pdf(wall_e::gram::simplify_rule_default(r1).to_relation_list(), "./app_out/r1_s");

    std::cout << "\n";

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
