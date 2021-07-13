#pragma once

#include <ostream>
#include <string>

#include "jsonh/enums.h"

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
}
