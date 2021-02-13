#include "jsonh/value.int.h"

#include "jsonh/visitor.h"

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
