#pragma once

#include <cstdint>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

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

            // if set, identifiers (including names with -) are considered strings
            IdentifierAsString = 1 << 3,

            Json = None
        };
    }

    namespace print_flags
    {
        enum Type
        {
            None = 0,

            // don't write commas
            SkipCommas = 1 << 1,

            // if string is a "identifier", don't add quotes
            StringAsIdent = 1 << 2,

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

    struct Document;

    struct Value;
    struct Object;
    struct Array;
    struct String;
    struct Number;
    struct Int;
    struct Bool;
    struct Null;

    enum class ValueType
    {
        Invalid,
        Object,
        Array,
        String,
        Number,
        Int,
        Bool,
        Null
    };

    struct Value
    {
        ValueType type;
        std::size_t index;

        Value();
        Value(ValueType vt, std::size_t i);

        bool is_valid() const;
        operator bool() const;
        bool operator!() const;

        // only exact matches
        Object* AsObject(Document* d);
        Array* AsArray(Document* d);
        String* AsString(Document* d);
        Number* AsNumber(Document* d);
        Int* AsInt(Document* d);
        Bool* AsBool(Document* d);
        Null* AsNull(Document* d);

        const Object* AsObject(const Document* d) const;
        const Array* AsArray(const Document* d) const;
        const String* AsString(const Document* d) const;
        const Number* AsNumber(const Document* d) const;
        const Int* AsInt(const Document* d) const;
        const Bool* AsBool(const Document* d) const;
        const Null* AsNull(const Document* d) const;
    };

    struct Object
    {
        Location location;
        std::map<std::string, Value> object;
    };

    struct Array
    {
        Location location;
        std::vector<Value> array;
    };

    struct String
    {
        Location location;
        std::string value;
    };

    struct Number
    {
        Location location;
        tnum value;
    };

    struct Int
    {
        Location location;
        tint value;
    };

    struct Bool
    {
        Location location;
        bool value;
    };

    struct Null
    {
        Location location;
    };

    struct Document
    {
        std::vector<Object> objects;
        std::vector<Array> arrays;
        std::vector<String> strings;
        std::vector<Number> numbers;
        std::vector<Int> ints;
        std::vector<Bool> bools;
        std::vector<Null> nulls;

        Value add(Object);
        Value add(Array);
        Value add(String);
        Value add(Number);
        Value add(Int);
        Value add(Bool);
        Value add(Null);
    };

    Location GetLocation(const Document* d, const Value& val);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Parse/print types

    struct ParseResult
    {
        // contains errors if parsing failed
        std::vector<Error> errors;

        // is non-null if parsing succeeded
        std::optional<Value> root;

        // the document
        Document doc;

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
    std::string Print(const Value& value, const Document* doc, print_flags::Type flags, const PrintStyle& pp);


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // streams

    std::ostream& operator<<(std::ostream& s, const ErrorType& type);
    std::ostream& operator<<(std::ostream& s, const Location& location);
    std::ostream& operator<<(std::ostream& s, const Error& error);
    std::ostream& operator<<(std::ostream& s, const ParseResult& result);
}
