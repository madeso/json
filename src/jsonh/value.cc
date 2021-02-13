#include "jsonh/value.h"

namespace jsonh
{
    Value::~Value() {}
    Object* Value::AsObject() { return nullptr; }
    Array* Value::AsArray() { return nullptr; }
    String* Value::AsString() { return nullptr; }
    Number* Value::AsNumber() { return nullptr; }
    Bool* Value::AsBool() { return nullptr; }
    Null* Value::AsNull() { return nullptr; }
    Int* Value::AsInt() { return nullptr; }
}
