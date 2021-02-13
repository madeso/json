#pragma once

#include <ostream>
#include <string>

namespace jsonh
{
    using tloc = size_t;

    struct Location
    {
        Location();
        Location(tloc l, tloc c);

        tloc line;
        tloc column;
    };

    std::ostream& operator<<(std::ostream& s, const Location& location);
}
