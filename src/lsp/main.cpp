


#include "ioserver.h"

#include <wall_e/src/flag.h>

#include <iostream>
#include "iofind.h"
#include <fstream>
#include <rapidjson/document.h>
//#include <libclsp/server.hpp>
//#include <libclsp/types.hpp>
#include <rapidjson/istreamwrapper.h>

std::ofstream logg = []{
    std::ofstream result;
    result.open("/home/borys/cpptest.log"); // append instead of overwrite
    return std::move(result);
}();

int exec_lsp(const wall_e::flag_provider &flags) {
    logg << "server start" << std::endl;
    ioserver server(std::cin, std::cout, logg);

    server.listen();
    /*
    clsp::Server server;

    clsp::Hover

    server.addRequest();

    server.startIO();

    // The object to save the parsed json
    clsp::Position p;

    // A handler that gets events from the reader
    clsp::JsonHandler handler;

    // This puts an empty initializer on the top of the stack of the handler.
    // And when the the handler gets the StartObject() a empty initializer is
    // pushed on the top of the handler stack.
    // Then the object fills the initializer with a map of keys to its setters.
    handler.objectStack.emplace().extraSetter =
    {
        // String
        {},

        // Number
        {},

        // Boolean
        {},

        // Null
        {},

        // Array
        {},

        // Object
        [&handler, &p]()
        {
            handler.pushInitializer();

            p.fillInitializer(handler.objectStack.top());
        }
    };



    // The json
    clsp::StringStream s("{\"line\":10,\"character\":2.5}");

    // The reader that sends events to the handler
    clsp::Reader r;

    // The parsing itself
    r.Parse(s, handler);

    // get<> is used because Number is an alias to variant<int, double>
    logg << "line:" << get<int>(p.line) << '\n';
    logg << "character:" << get<double>(p.character) << '\n';
*/
    return 0;
}

void example() {


    // The object to save the parsed json
    clsp::Position p;

    // A handler that gets events from the reader
    clsp::JsonHandler handler;

    // This puts an empty initializer on the top of the stack of the handler.
    // And when the the handler gets the StartObject() a empty initializer is
    // pushed on the top of the handler stack.
    // Then the object fills the initializer with a map of keys to its setters.
    handler.objectStack.emplace().extraSetter =
    {
        // String
        {},

        // Number
        {},

        // Boolean
        {},

        // Null
        {},

        // Array
        {},

        // Object
        [&handler, &p]()
        {
            handler.pushInitializer();

            p.fillInitializer(handler.objectStack.top());
        }
    };

    // The json
    clsp::StringStream s("{\"line\":10,\"character\":2.5}");

    // The reader that sends events to the handler
    clsp::Reader r;

    // The parsing itself
    r.Parse(s, handler);

    // get<> is used because Number is an alias to variant<int, double>
    std::cout << "line:" << get<int>(p.line) << '\n';
    std::cout << "character:" << get<double>(p.character) << '\n';

}


void parse_file(const char* file) {



    // The object to save the parsed json
    clsp::InitializeParams initializeParams;

    // A handler that gets events from the reader
    clsp::JsonHandler handler;

    // This puts an empty initializer on the top of the stack of the handler.
    // And when the the handler gets the StartObject() a empty initializer is
    // pushed on the top of the handler stack.
    // Then the object fills the initializer with a map of keys to its setters.
    handler.objectStack.emplace().extraSetter =
    {
        // String
        {},

        // Number
        {},

        // Boolean
        {},

        // Null
        {},

        // Array
        {},

        // Object
        [&handler, &initializeParams]()
        {
            handler.pushInitializer();

            initializeParams.fillInitializer(handler.objectStack.top());
        }
    };



    logg << "file: '" << file << "'" << std::endl;
    std::ifstream ifs(file);
    rapidjson::IStreamWrapper isw(ifs);

    clsp::Reader reader;

    reader.Parse(isw, handler);

    if(reader.HasParseError()) {
        logg << "err: " << ioserver::err_to_str(reader.GetParseErrorCode()) << ":" << reader.GetErrorOffset() << std::endl;
    } else {
        logg.flush();
    }






    std::cout << "initializeParams.processId: " << streamer{ initializeParams.processId } << std::endl;
}




int main(int argc, char** argv) {
    wall_e::flag_provider flags(argc, argv);

    const auto stdio = flags.bool_flag(wall_e::flag::name_from_string("stdio"), "Use as stdio server");
    const auto hang = flags.bool_flag(wall_e::flag::name_from_string("hang"), "hang server");

    if(stdio.bool_data()) {
        exec_lsp(flags);
    } else if(hang.bool_data()) {
        while(1) {}
    } else {

        parse_file("/home/borys/default_body2.json");

        parse_file("/home/borys/default_request.json");
        //example();
        std::cerr << "use --stdio flag" << std::endl;
        return 1;
    }
}
