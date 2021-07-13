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

    char CharToHex(char c);
    void SkipSpaces(Parser* parser);
    void AppendChar(std::ostream& s, char c);

    constexpr bool IsSpace(char c)
    {
        if (c == ' ')
            return true;
        if (c == '\t')
            return true;
        if (c == '\n')
            return true;
        if (c == '\r')
            return true;
        return false;
    }

    constexpr bool IsDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    constexpr bool IsHex(char c)
    {
        return IsDigit(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    constexpr bool IsValidFirstDigit(char c)
    {
        if (c == '-')
            return true;
        if (IsDigit(c))
            return true;
        return false;
    }
}
