#include "jsonh/value.object.h"

#include "jsonh/visitor.h"

namespace jsonh
{
    void Object::Visit(Visitor* visitor)
    {
        visitor->VisitObject(this);
    }

    Object* Object::AsObject()
    {
        return this;
    }
}
