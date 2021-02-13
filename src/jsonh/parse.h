#pragma once

#include "jsonh/parse.result.h"

namespace jsonh
{
    namespace ParseFlags
    {
        enum Type
        {
            None = 0,
            DuplicateKeysOnlyLatest = 1 << 1,

            Json = None
        };
    }

    ParseResult Parse(const std::string& str, ParseFlags::Type flags);
}
