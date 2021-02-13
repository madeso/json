#pragma once

#include <ostream>
#include <string>

#include "jsonh/error.h"
#include "jsonh/value.array.h"
#include "jsonh/value.object.h"
#include "jsonh/value.string.h"

namespace jsonh
{
    struct Parser;
    struct ParseResult;
    struct Location;

    bool IsSpace(char c);
    bool IsDigit(char c);
    bool IsHex(char c);
    bool IsValidFirstDigit(char c);
    char CharToHex(char c);

    void SkipSpaces(Parser* parser);

    void AppendChar(std::ostream& s, char c);

    void AddError(ParseResult* result, Parser* parser, Error::Type type, const std::string& err);
    void AddNote(ParseResult* result, const Location& loc, const std::string& note);

    std::shared_ptr<Value> ParseValue(ParseResult* result, Parser* parser);
    std::shared_ptr<Object> ParseObject(ParseResult* result, Parser* parser);
    std::shared_ptr<Array> ParseArray(ParseResult* result, Parser* parser);
    std::shared_ptr<String> ParseString(ParseResult* result, Parser* parser);
    std::shared_ptr<Value> ParseNumber(ParseResult* result, Parser* parser);

    bool ParseEscapeCode(ParseResult* result, Parser* parser, std::ostringstream& ss);

    ParseResult Parse(Parser* parser);
}
