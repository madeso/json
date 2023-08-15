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
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    using tint = int64_t;
    using tnum = double;
    using tloc = std::size_t;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

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

    namespace parse_flags
    {
        enum Type
        {
            None = 0,

            /// if set duplicate keys are handled by ignoring all but the latest
            /// if not set, duplicate keys are considered a error
            DuplicateKeysOnlyLatest = 1 << 1,

            // if set, all commas are optional
            IgnoreAllCommas = 1 << 2,

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

    struct Error
    {
        ErrorType type;
        std::string message;
        Location location;

        Error(ErrorType t, const std::string& m, const Location& l = Location{});
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // actual JSON types

    struct Visitor;

    struct Value;
    struct Object;
    struct Array;
    struct String;
    struct Number;
    struct Int;
    struct Bool;
    struct Null;

    struct Value
    {
        Location location;

        // only exact matches
        virtual Object* AsObject();
        virtual Array* AsArray();
        virtual String* AsString();
        virtual Number* AsNumber();
        virtual Int* AsInt();
        virtual Bool* AsBool();
        virtual Null* AsNull();

        virtual void Visit(Visitor* visitor) = 0;
        virtual ~Value() = default;
    };

    struct Object : public Value
    {
        std::map<std::string, std::unique_ptr<Value>> object;

        void Visit(Visitor* visitor) override;
        Object* AsObject() override;
    };

    struct Array : public Value
    {
        std::vector<std::unique_ptr<Value>> array;

        void Visit(Visitor* visitor) override;
        Array* AsArray() override;
    };

    struct String : public Value
    {
        std::string string;

        void Visit(Visitor* visitor) override;
        String* AsString() override;
        explicit String(const std::string& s = "");
    };

    struct Number : public Value
    {
        tnum number;

        void Visit(Visitor* visitor) override;
        Number* AsNumber() override;
        explicit Number(tnum d);
    };

    struct Int : public Value
    {
        tint integer;

        void Visit(Visitor* visitor) override;
        Int* AsInt() override;
        explicit Int(tint i);
    };

    struct Bool : public Value
    {
        bool boolean;

        void Visit(Visitor* visitor) override;
        Bool* AsBool() override;
        explicit Bool(bool b);
    };

    struct Null : public Value
    {
        void Visit(Visitor* visitor) override;
        Null* AsNull() override;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Parse/print types

    struct ParseResult
    {
        // contains errors if parsing failed
        std::vector<Error> errors;

        // is non-null if parsing succeeded
        std::unique_ptr<Value> value;

        [[nodiscard]] bool HasError() const;
        operator bool() const;
    };

    struct PrintStyle
    {
        std::string_view indent;
        std::string_view space;
        std::string_view newline;
    };

    constexpr PrintStyle Pretty = PrintStyle{"  ", " ", "\n"};
    constexpr PrintStyle Compact = PrintStyle{"", "", ""};

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Parse and print functions

    ParseResult Parse(const std::string& str, parse_flags::Type flags);
    std::string Print(Value* value, print_flags::Type flags, const PrintStyle& pp);

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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // streams

    std::ostream& operator<<(std::ostream& s, const ErrorType& type);
    std::ostream& operator<<(std::ostream& s, const Location& location);
    std::ostream& operator<<(std::ostream& s, const Error& error);
    std::ostream& operator<<(std::ostream& s, const ParseResult& result);
}
