#include "jsonh/value.array.h"

#include "jsonh/visitor.h"

namespace jsonh
{
    void Array::Visit(Visitor* visitor)
    {
        visitor->VisitArray(this);
    }

    Array* Array::AsArray()
    {
        return this;
    }
}
