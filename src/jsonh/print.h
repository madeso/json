#pragma once

#include <string>

namespace jsonh
{
    struct Value;

    struct PrettyPrint
    {
        std::string indent;
        std::string space;
        std::string newline;
        static PrettyPrint Pretty();
        static PrettyPrint Compact();
    };

    namespace PrintFlags
    {
        enum Type
        {
            None = 0,

            Json = None
        };
    }

    std::string Print(Value* value, PrintFlags::Type flags, const PrettyPrint& pp);
}
