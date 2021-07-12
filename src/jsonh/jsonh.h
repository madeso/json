#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace jsonh
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    using tloc = size_t;
    using tint = int64_t;
    using tnum = double;

    struct Visitor;

    struct Value;
    struct Object;
    struct Array;
    struct String;
    struct Number;
    struct Bool;
    struct Null;
    struct Int;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Location
    {
        Location();
        Location(tloc l, tloc c);

        tloc line;
        tloc column;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Error
    {
        enum class Type
        {
            Note,
            InvalidCharacter,
            UnclosedArray,
            UnclosedObject,
            NotEof,
            InvalidNumber,
            IllegalEscape,
            InvalidCharacterInString,
            UnexpectedEof,
            DuplicateKey,
            UnknownError
        };
        Type type;
        std::string message;
        Location location;

        Error(Type t, const std::string& m, const Location& l = Location());
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct ParseResult
    {
        // contains errors if parsing failed
        std::vector<Error> errors;

        // is non-null is parsing succeeded
        std::unique_ptr<Value> value;

        bool HasError() const;

        operator bool() const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    namespace ParseFlags
    {
        enum Type
        {
            None = 0,
            DuplicateKeysOnlyLatest = 1 << 1,

            Json = None
        };
    }

    ParseResult Parse(const std::string& str, ParseFlags::Type flags);

    struct PrettyPrint
    {
        std::string indent;
        std::string space;
        std::string newline;
        static PrettyPrint Pretty();
        static PrettyPrint Compact();
    };

    namespace PrintFlags
    {
        enum Type
        {
            None = 0,

            Json = None
        };
    }

    std::string Print(Value* value, PrintFlags::Type flags, const PrettyPrint& pp);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Array : public Value
    {
        std::vector<std::unique_ptr<Value>> array;

        void Visit(Visitor* visitor) override;

        Array* AsArray() override;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Bool : public Value
    {
        bool boolean;

        void Visit(Visitor* visitor) override;

        Bool* AsBool() override;

        explicit Bool(bool b);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Int : public Value
    {
        tint integer;

        void Visit(Visitor* visitor) override;

        Int* AsInt() override;

        explicit Int(tint i);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Null : public Value
    {
        void Visit(Visitor* visitor) override;

        Null* AsNull() override;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Number : public Value
    {
        tnum number;

        void Visit(Visitor* visitor) override;

        Number* AsNumber() override;

        explicit Number(tnum d);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Object : public Value
    {
        std::map<std::string, std::unique_ptr<Value>> object;

        void Visit(Visitor* visitor) override;

        Object* AsObject() override;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct String : public Value
    {
        std::string string;

        void Visit(Visitor* visitor) override;

        String* AsString() override;

        explicit String(const std::string& s = "");
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

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

    std::ostream& operator<<(std::ostream& s, const Location& location);
    std::ostream& operator<<(std::ostream& s, const Error::Type& type);
    std::ostream& operator<<(std::ostream& s, const Error& error);
    std::ostream& operator<<(std::ostream& s, const ParseResult& result);

}

#ifdef JSONH_IMPLEMENTATION

#include "jsonh/detail.inl"
#include "jsonh/jsonh.inl"

#endif
