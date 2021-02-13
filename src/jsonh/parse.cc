#include "jsonh/parse.h"

#include "internal.parse_functions.h"
#include "internal.parser.h"

namespace jsonh
{
    ParseResult Parse(const std::string& str, ParseFlags::Type flags)
    {
        Parser parser{str, flags};
        return Parse(&parser);
    }
}
