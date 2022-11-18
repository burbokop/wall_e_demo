#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <src/gui/appcore.h>
#include <src/gui/kgramtreeview.h>
#include <src/gui/presentation/presentor.h>
#include <src/gui/presentation/textedithover.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <QQmlContext>
#include <wall_e/src/flag.h>
#include <wall_e/src/color.h>
#include <src/to_pdf.h>
#include <sproc/src/apt.h>
#include <wall_e/src/gram.h>

#include <sproc/src/environment.h>
#include <fstream>
#include <src/km2/backend/unit/unit.h>
#include <type_traits>
#include <src/km2/tree/block_node.h>
#include <src/km2/tree/call_node.h>
#include <src/km2/cpp/cpp_parser.h>
#include <wall_e/src/models/compiler_info.h>
#include <wall_e/src/stack.h>
#include <src/gui/log/logtablemodel.h>
#include <src/km2/lsp/service.h>

void do_clang() {
#ifdef CLANG_ENABLED
    cpp_parser ppp(argc, argv);

    const auto cppcode = "\
        namespace ooo {           \n\
            int foo(int a) {      \n\
                return a;         \n\
            }                     \n\
            int main() {          \n\
                return foo(2);    \n\
            }                     \n\
        }                         \n\
        ";

    if(additional_log) std::cout << "SOURCE CODE" << std::endl << cppcode << std::endl << "SOURCE CODE END" << std::endl;

    const auto& clang_result = ppp.parse(cppcode, "input.cc");

    if(additional_log) {
        std::cout << "CLNG RESULT" << std::endl;
        if(clang_result.node) {

            if(const auto& km2_node = clang_result.node.km2_node()) {
                km2_node->print(0, std::cout);
            } else {
                std::cout << "empty km2 root node" << std::endl;
            }
        } else {
            std::cout << "empty clang root node" << std::endl;
        }
        std::cout << "SOURCE CODE END" << std::endl;
    }
#endif
}

int main(int argc, char **argv) {


    std::cout << "wall_e compiled with: " << wall_e::cxx_info << std::endl;
    std::cout << "km2 compiled with: " << wall_e::inline_cxx_info << std::endl;

    const bool additional_log = false;

    using namespace std::chrono_literals;
    do_clang();

    //return clang_result.status;

    //km2::module_builder b;
    //b.aaa();
    //b.print();
    //b.runJit();
    //
    //return 0;

    /*

    MODULE:
    ; ModuleID = 'Module'
    source_filename = "Module"

    @.fmt = constant [17 x i8] c"Hello World: %d\0A\00"

    define i32 @sum(i32 %0, i32 %1) {
    entry:
      %sum2 = add i32 2, %0
      %result = add i32 %sum2, %1
      ret i32 %result
    }

    define i32 @main() {
    entry:
      %0 = call i32 @sum(i32 10, i32 20)
      %1 = call i32 (i8*, ...) @printf([17 x i8]* @.fmt, i32 %0)
      ret i32 %0
    }

    declare i32 @printf(i8*, ...)

    */


    wall_e::gram::rule::assignTypeSymbol(wall_e::gram::rule_type::Conjunction, '&');
    wall_e::gram::rule::assignTypeSymbol(wall_e::gram::rule_type::Disjunction, '|');

    if(additional_log) {
        std::cout << "before wall_e::gram::rule_from_str" << std::endl;
        const auto r0 = (wall_e::gram::rule("decl_arg") & (wall_e::gram::rule("EP") | (wall_e::gram::rule("COMA") & "decl_arg_list")));
        std::cout << "after wall_e::gram::rule_from_str" << std::endl;
        const auto r1 = ((wall_e::gram::rule("cmd") & "SEMICOLON") & (wall_e::gram::rule("EB") | "internal_block"));


        wall_e::write_graph(std::cout, r0.to_graph());

        std::cout << r0 << std::endl;
        wall_e::write_graph(std::cout, r0.to_graph());
        std::cout << "sm: " << wall_e::gram::simplify_rule_default(r0) << std::endl;
        wall_e::write_graph(std::cout, wall_e::gram::simplify_rule_default(r0).to_graph());

        std::cout << r1 << std::endl;
        wall_e::write_graph(std::cout, r1.to_graph());
        std::cout << "sm: " << wall_e::gram::simplify_rule_default(r1) << std::endl;
        wall_e::write_graph(std::cout, wall_e::gram::simplify_rule_default(r1).to_graph());

        std::filesystem::create_directory("app_out");
        to_pdf(r0.to_relation_list(), "./app_out/r0");
        to_pdf(r1.to_relation_list(), "./app_out/r1");
        to_pdf(wall_e::gram::simplify_rule_default(r0).to_relation_list(), "./app_out/r0_s");
        to_pdf(wall_e::smp::cc(r0).to_relation_list(), "./app_out/r0_s2");
        to_pdf(wall_e::gram::simplify_rule_default(r1).to_relation_list(), "./app_out/r1_s");
        to_pdf(wall_e::smp::cc(r1).to_relation_list(), "./app_out/r1_s2");

        std::cout << std::endl;
        std::cout << "____" << std::endl;
    }

    qmlRegisterType<KGramTreeView>("Km2", 1, 0, "KGramTreeView");
    qmlRegisterType<ProjFile>("Km2", 1, 0, "ProjFile");
    qmlRegisterType<Compiler>("Km2", 1, 0, "Compiler");
    qmlRegisterType<Presentor>("Km2", 1, 0, "Presentor");
    qmlRegisterType<LogTableModel>("Km2", 1, 0, "LogTableModel");
    qmlRegisterType<TextEditHover>("Km2", 1, 0, "TextEditHover");

    qmlRegisterUncreatableType<BackendFactory>("Km2", 1, 0, "BackendFactory", "created in appcore");
    qmlRegisterUncreatableType<JitExecutor>("Km2", 1, 0, "JitExecutor", "created in appcore");
    qmlRegisterUncreatableType<TokenTree>("Km2", 1, 0, "TokenTree", "created in compiler");

    QCoreApplication::setOrganizationName("burbokop");
    QCoreApplication::setOrganizationDomain("io.github.burbokop");

    Q_INIT_RESOURCE(resources);

    QGuiApplication app(argc, argv);
    AppCore appCore(&app);
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appCore", &appCore);
    engine.load("qrc:/resources/main.qml");

    return app.exec();
}
