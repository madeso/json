#include "jsonh/jsonh.h"

namespace jsonh
{
    void Bool::Visit(Visitor* visitor)
    {
        visitor->VisitBool(this);
    }

    Bool* Bool::AsBool()
    {
        return this;
    }

    Bool::Bool(bool b)
        : boolean(b)
    {
    }
}
