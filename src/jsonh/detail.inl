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

    bool IsSpace(char c)
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

    void SkipSpaces(Parser* parser)
    {
        while (IsSpace(parser->Peek()))
        {
            parser->Read();
        }
    }

    void AppendChar(std::ostream& s, char c)
    {
        switch (c)
        {
        case '\n':
            s << "<newline>";
            break;
        case '\r':
            s << "<linefeed>";
            break;
        case 0:
            s << "<EOF>";
            break;
        case '\t':
            s << "<tab>";
            break;
        case ' ':
            s << "<space>";
            break;
        default:
            s << c << "(" << static_cast<int>(static_cast<unsigned char>(c)) << ")";
            break;
        }
    }

    bool IsDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool IsHex(char c)
    {
        return IsDigit(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
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

    bool IsValidFirstDigit(char c)
    {
        if (c == '-')
            return true;
        if (IsDigit(c))
            return true;
        return false;
    }
}
