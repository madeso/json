#include <cassert>
#include <sstream>

#include "jsonh/detail.h"
#include "jsonh/jsonh.h"

namespace jsonh::detail
{
    Parser::Parser(const std::string& s, ParseFlags::Type f)
        : str(s), flags(f)
    {
    }

    char Parser::Peek(tloc advance) const
    {
        const tloc i = index + advance;
        if (index >= str.size())
            return 0;
        return str[i];
    }

    bool Parser::HasMoreChar() const
    {
        return index < str.size();
    }

    Location Parser::GetLocation() const
    {
        return {line, column};
    }

    char Parser::Read()
    {
        const char c = str[index];
        if (c == '\n')
        {
            line += 1;
            column = 0;
        }
        else
        {
            column += 1;
        }
        index += 1;
        return c;
    }

    void SkipSpaces(Parser* parser)
    {
        while (IsSpace(parser->Peek()))
        {
            parser->Read();
        }
    }

    char CharToHex(char c)
    {
        if (c >= '0' && c <= '9')
        {
            return c - '0';
        }
        else if (c >= 'a' && c <= 'f')
        {
            return 10 + (c - 'a');
        }
        else if (c >= 'A' && c <= 'F')
        {
            return 10 + (c - 'A');
        }
        else
        {
            assert(false && "Invalid hex code");
            return 0;
        }
    }
}
