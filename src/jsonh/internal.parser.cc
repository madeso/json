#include "internal.parser.h"

namespace jsonh
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
}
