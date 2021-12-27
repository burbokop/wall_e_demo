#include "ioserver.h"
#include <libclsp/server/jsonHandler.hpp>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <thread>
#include <boost/iostreams/copy.hpp>
#include "iofind.h"

std::ostream& operator << (std::ostream& stream, const clsp::ObjectInitializer& i) {
    stream << "-----------------------" << std::endl;
    stream << "key: " << i.key << std::endl;
    stream << "key: " << i.setterMap.size() << std::endl;
    stream << "end --------------- end" << std::endl;
    return stream;
}



ioserver::ioserver(std::istream &istream, std::ostream &ostream, std::ostream& log)
    : m_istream(istream),
      m_ostream(ostream),
      m_log(log)
{}

void ioserver::onInitialize(std::function<clsp::InitializeResult (const clsp::InitializeParams &)> handler) {
    m_onInitialize = handler;
}

void ioserver::onInitialized(std::function<void (const clsp::InitializedParams &)> handler) {
    m_onInitialized = handler;
}

void ioserver::onCompletion(std::function<clsp::CompletionList (const clsp::CompletionParams &)> handler) {
    m_onCompletion = handler;
}

void ioserver::onHover(std::function<clsp::Hover (const clsp::TextDocumentPositionParams &)> handler) {
    m_onHover = handler;
}

void ioserver::parse(const std::string &string) {

    clsp::JsonHandler handler;

    //auto& setterMap = handler.objectStack.emplace().setterMap;

    clsp::InitializeParams initializeParams;
    handler.objectStack.emplace().extraSetter = clsp::ValueSetter {
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
    [this, &handler, &initializeParams]()
    {
        handler.pushInitializer();
        initializeParams.fillInitializer(handler.objectStack.top());
    }
};

    //handler.objectStack.emplace().extraSetter = ssss;
    //handler.objectStack.emplace().extraSetter = ssss;
    //handler.objectStack.emplace().extraSetter = ssss;

    m_log << "body: '" << string << "'" << std::endl;
    rapidjson::StringStream ss(string.c_str());

    clsp::Reader reader;

    reader.Parse(ss, handler);

    if(reader.HasParseError()) {
        m_log << "err: " << err_to_str(reader.GetParseErrorCode()) << std::endl;
    } else {
        m_log.flush();
    }


    m_log << "initializeParams.processId: " << streamer{ initializeParams.processId } << std::endl;
}


void ioserver::listen() {
    m_log << "listen" << std::endl;
    while(1) {
        if(false) {
            char c;
            if(m_istream >> c) {
                m_log << c;
                m_log.flush();
            }
        } else {
            iofind(m_istream, "Content-Length:");
            std::size_t length;
            m_istream >> length;
            char buffer[length + 1];
            read(m_istream, buffer, length);
            buffer[length] = '\0';
            parse(std::string(buffer, length));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
