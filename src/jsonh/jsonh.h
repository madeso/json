#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace jsonh
{
    using tint = int64_t;
    using tnum = double;
    using tloc = std::size_t;

    enum class ErrorType
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

    std::ostream& operator<<(std::ostream& s, const ErrorType& type);

    namespace parse_flags
    {
        enum Type
        {
            None = 0,
            DuplicateKeysOnlyLatest = 1 << 1,

            Json = None
        };
    }

    namespace print_flags
    {
        enum Type
        {
            None = 0,

            Json = None
        };
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Location
    {
        constexpr Location()
            : line(0)
            , column(0)
        {
        }

        constexpr Location(tloc l, tloc c)
            : line(l)
            , column(c)
        {
        }

        tloc line;
        tloc column;
    };

    std::ostream& operator<<(std::ostream& s, const Location& location);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Error
    {
        ErrorType type;
        std::string message;
        Location location;

        Error(ErrorType t, const std::string& m, const Location& l = Location{});
    };

    std::ostream& operator<<(std::ostream& s, const Error& error);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

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

    struct Value
    {
        Location location;

        virtual ~Value() = default;

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

        [[nodiscard]] constexpr bool HasError() const { return value == nullptr; }
        constexpr operator bool() const { return !HasError(); }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    ParseResult Parse(const std::string& str, parse_flags::Type flags);

    struct PrintStyle
    {
        std::string_view indent;
        std::string_view space;
        std::string_view newline;
    };

    constexpr PrintStyle Pretty = PrintStyle{"  ", " ", "\n"};
    constexpr PrintStyle Compact = PrintStyle{"", "", ""};

    std::string Print(Value* value, print_flags::Type flags, const PrintStyle& pp);

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

    std::ostream& operator<<(std::ostream& s, const ParseResult& result);

    std::string Print(Value* value, print_flags::Type, const PrintStyle& pp);

    ParseResult Parse(const std::string& str, parse_flags::Type flags);
}
