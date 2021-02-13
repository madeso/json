#include "jsonh/value.null.h"

#include "jsonh/visitor.h"

namespace jsonh
{
    void Null::Visit(Visitor* visitor)
    {
        visitor->VisitNull(this);
    }

    Null* Null::AsNull()
    {
        return this;
    }
}
