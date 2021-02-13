#pragma once

#include "jsonh/parse.h"
#include "jsonh/parse.result.h"

namespace jsonh
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
}
