#include "jsonh/value.number.h"

#include "jsonh/visitor.h"

namespace jsonh
{
    void Number::Visit(Visitor* visitor)
    {
        visitor->VisitNumber(this);
    }

    Number* Number::AsNumber()
    {
        return this;
    }

    Number::Number(tnum d)
        : number(d)
    {
    }
}
