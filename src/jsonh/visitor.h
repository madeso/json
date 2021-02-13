#pragma once

namespace jsonh
{
struct Value;
struct Object;
struct Array;
struct String;
struct Number;
struct Bool;
struct Null;
struct Int;

struct Visitor
{
    // will not recurse, if you want to visit the children, you have to keep calling Visit
    virtual void VisitObject(Object* object) = 0;
    virtual void VisitArray(Array* array) = 0;

    virtual void VisitString(String* string) = 0;
    virtual void VisitNumber(Number* number) = 0;
    virtual void VisitBool(Bool* boolean) = 0;
    virtual void VisitNull(Null* null) = 0;
    virtual void VisitInt(Int* integer) = 0;
};

}
