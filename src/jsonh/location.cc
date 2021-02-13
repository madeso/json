#include "jsonh/location.h"

namespace jsonh
{
    Location::Location()
        : line(0), column(0)
    {
    }

    Location::Location(tloc l, tloc c)
        : line(l), column(c)
    {
    }

    std::ostream& operator<<(std::ostream& s, const Location& location)
    {
        s << "(" << location.line << ", " << location.column << ")";
        return s;
    }
}
