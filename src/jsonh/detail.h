#pragma once

#include <ostream>
#include <string>

#include "jsonh/jsonh.h"

namespace jsonh::detail
{
    struct Parser
    {
        std::string str;
        ParseFlags::Type flags;
        tloc index = 0;

        Parser(const std::string& s, ParseFlags::Type f);

        char Peek(tloc advance = 0) const;

        bool HasMoreChar() const;

        tloc line = 1;
        tloc column = 0;

        Location GetLocation() const;

        char Read();
    };

    bool IsSpace(char c);
    bool IsDigit(char c);
    bool IsHex(char c);
    bool IsValidFirstDigit(char c);
    char CharToHex(char c);

    void SkipSpaces(Parser* parser);

    void AppendChar(std::ostream& s, char c);

    void AddError(ParseResult* result, Parser* parser, Error::Type type, const std::string& err);
    void AddNote(ParseResult* result, const Location& loc, const std::string& note);

    std::unique_ptr<Value> ParseValue(ParseResult* result, Parser* parser);
    std::unique_ptr<Object> ParseObject(ParseResult* result, Parser* parser);
    std::unique_ptr<Array> ParseArray(ParseResult* result, Parser* parser);
    std::unique_ptr<String> ParseString(ParseResult* result, Parser* parser);
    std::unique_ptr<Value> ParseNumber(ParseResult* result, Parser* parser);

    bool ParseEscapeCode(ParseResult* result, Parser* parser, std::ostringstream& ss);

    ParseResult Parse(Parser* parser);
}
