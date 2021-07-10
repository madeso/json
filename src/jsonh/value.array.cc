#include "jsonh/jsonh.h"

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
