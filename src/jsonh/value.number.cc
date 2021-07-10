#include "jsonh/jsonh.h"

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
