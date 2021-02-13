#pragma once

#include "jsonh/location.h"

namespace jsonh
{
struct Visitor;

struct Value;
struct Object;
struct Array;
struct String;
struct Number;
struct Bool;
struct Null;
struct Int;

struct Value
{
    Location location;

    virtual ~Value();

    virtual void Visit(Visitor* visitor) = 0;

    // only exact matches
    virtual Object* AsObject();
    virtual Array* AsArray();
    virtual String* AsString();
    virtual Number* AsNumber();
    virtual Bool* AsBool();
    virtual Null* AsNull();
    virtual Int* AsInt();
};

}
