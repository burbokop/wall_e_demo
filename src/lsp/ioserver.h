#ifndef IOSERVER_H
#define IOSERVER_H

#include <iosfwd>
#include <functional>
#include <libclsp/types.hpp>
#include <rapidjson/stream.h>

struct ioserver {
    class null_buffer : public std::streambuf {
    public:
        virtual int overflow(int c) override { return c; }
    };
private:
    static inline null_buffer null_buffer;
public:
    static inline std::ostream null_stream = std::ostream(&null_buffer);
private:

    std::istream &m_istream;
    std::ostream &m_ostream;
    std::ostream &m_log;

    std::function<clsp::InitializeResult(const clsp::InitializeParams&)> m_onInitialize;
    std::function<void(const clsp::InitializedParams&)> m_onInitialized;

    std::function<clsp::CompletionList(const clsp::CompletionParams&)> m_onCompletion;
    std::function<clsp::Hover(const clsp::TextDocumentPositionParams&)> m_onHover;
public:
    ioserver(std::istream &istream, std::ostream &ostream, std::ostream& log = null_stream);

    void onInitialize(std::function<clsp::InitializeResult(const clsp::InitializeParams&)> handler);
    void onInitialized(std::function<void(const clsp::InitializedParams&)> handler);

    void onCompletion(std::function<clsp::CompletionList(const clsp::CompletionParams&)> handler);
    void onHover(std::function<clsp::Hover(const clsp::TextDocumentPositionParams&)> handler);

    void parse(const std::string& string);

    static void proceed_pipe(std::istream &i, std::ostream &o) {
        char c;
        while(i >> c) {
            o << c;
            o.flush();
        }
    }

    static void read(std::istream &istream, char* buffer, std::size_t s) {
        std::size_t i = 0;
        while(i < s) {
            istream >> buffer[i++];
        }
    }

    static std::string err_to_str(clsp::ParseErrorCode code) {
        switch(code) {
        case clsp::kParseErrorNone: return "No error.";
        case clsp::kParseErrorDocumentEmpty: return "The document is empty.";
        case clsp::kParseErrorDocumentRootNotSingular: return "The document root must not follow by other values.";
        case clsp::kParseErrorValueInvalid: return "Invalid value.";
        case clsp::kParseErrorObjectMissName: return "Missing a name for object member.";
        case clsp::kParseErrorObjectMissColon: return "Missing a colon after a name of object member.";
        case clsp::kParseErrorObjectMissCommaOrCurlyBracket: return "Missing a comma or '}' after an object member.";
        case clsp::kParseErrorArrayMissCommaOrSquareBracket: return "Missing a comma or ']' after an array element.";
        case clsp::kParseErrorStringUnicodeEscapeInvalidHex: return "Incorrect hex digit after \\u escape in string.";
        case clsp::kParseErrorStringUnicodeSurrogateInvalid: return "The surrogate pair in string is invalid.";
        case clsp::kParseErrorStringEscapeInvalid: return "Invalid escape character in string.";
        case clsp::kParseErrorStringMissQuotationMark: return "Missing a closing quotation mark in string.";
        case clsp::kParseErrorStringInvalidEncoding: return "Invalid encoding in string.";
        case clsp::kParseErrorNumberTooBig: return "Number too big to be stored in double.";
        case clsp::kParseErrorNumberMissFraction: return "Miss fraction part in number.";
        case clsp::kParseErrorNumberMissExponent: return "Miss exponent in number.";
        case clsp::kParseErrorTermination: return "Parsing was terminated.";
        case clsp::kParseErrorUnspecificSyntaxError: return "Unspecific syntax error.";
        };
    }

    void listen();
};

#endif // IOSERVER_H
