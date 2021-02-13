#include "jsonh/value.string.h"

#include "jsonh/visitor.h"

namespace jsonh
{
    void String::Visit(Visitor* visitor)
    {
        visitor->VisitString(this);
    }

    String* String::AsString()
    {
        return this;
    }

    String::String(const std::string& s)
        : string(s)
    {
    }
}
