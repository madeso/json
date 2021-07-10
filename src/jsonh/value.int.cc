#include "jsonh/jsonh.h"

namespace jsonh
{
    void Int::Visit(Visitor* visitor)
    {
        visitor->VisitInt(this);
    }

    Int* Int::AsInt()
    {
        return this;
    }

    Int::Int(tint i)
        : integer(i)
    {
    }
}
