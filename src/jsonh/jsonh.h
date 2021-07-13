#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "jsonh/detail.h"
#include "jsonh/enums.h"

namespace jsonh
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    using default_tint = int64_t;
    using default_tnum = double;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct TVisitor;

    template <typename tint, typename tnum>
    struct TValue;

    template <typename tint, typename tnum>
    struct TObject;

    template <typename tint, typename tnum>
    struct TArray;

    template <typename tint, typename tnum>
    struct TString;

    template <typename tint, typename tnum>
    struct TNumber;

    template <typename tint, typename tnum>
    struct TBool;

    template <typename tint, typename tnum>
    struct TNull;

    template <typename tint, typename tnum>
    struct TInt;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    using Visitor = TVisitor<default_tint, default_tnum>;
    using Value = TValue<default_tint, default_tnum>;
    using Object = TObject<default_tint, default_tnum>;
    using Array = TArray<default_tint, default_tnum>;
    using String = TString<default_tint, default_tnum>;
    using Number = TNumber<default_tint, default_tnum>;
    using Bool = TBool<default_tint, default_tnum>;
    using Null = TNull<default_tint, default_tnum>;
    using Int = TInt<default_tint, default_tnum>;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct TValue
    {
        Location location;

        virtual ~TValue() = default;

        virtual void Visit(TVisitor<tint, tnum>* visitor) = 0;

        // only exact matches
        virtual TObject<tint, tnum>* AsObject() { return nullptr; }
        virtual TArray<tint, tnum>* AsArray() { return nullptr; }
        virtual TString<tint, tnum>* AsString() { return nullptr; }
        virtual TNumber<tint, tnum>* AsNumber() { return nullptr; }
        virtual TBool<tint, tnum>* AsBool() { return nullptr; }
        virtual TNull<tint, tnum>* AsNull() { return nullptr; }
        virtual TInt<tint, tnum>* AsInt() { return nullptr; }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct ParseResult
    {
        // contains errors if parsing failed
        std::vector<Error> errors;

        // is non-null is parsing succeeded
        std::unique_ptr<TValue<tint, tnum>> value;

        bool HasError() const { return value == nullptr; }
        operator bool() const { return !HasError(); }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint = default_tint, typename tnum = default_tnum>
    ParseResult<tint, tnum> Parse(const std::string& str, ParseFlags::Type flags);

    // todo(Gustav): rename to PrintStyle
    struct PrettyPrint
    {
        std::string indent;
        std::string space;
        std::string newline;
        static PrettyPrint Pretty();
        static PrettyPrint Compact();
    };

    template <typename tint, typename tnum>
    std::string Print(TValue<tint, tnum>* value, PrintFlags::Type flags, const PrettyPrint& pp);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct TArray : public TValue<tint, tnum>
    {
        std::vector<std::unique_ptr<TValue<tint, tnum>>> array;

        void Visit(TVisitor<tint, tnum>* visitor) override;

        TArray<tint, tnum>* AsArray() override;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct TBool : public TValue<tint, tnum>
    {
        bool boolean;

        void Visit(TVisitor<tint, tnum>* visitor) override;

        TBool<tint, tnum>* AsBool() override;

        explicit TBool(bool b);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct TInt : public TValue<tint, tnum>
    {
        tint integer;

        void Visit(TVisitor<tint, tnum>* visitor) override;

        TInt<tint, tnum>* AsInt() override;

        explicit TInt(tint i);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct TNull : public TValue<tint, tnum>
    {
        void Visit(TVisitor<tint, tnum>* visitor) override;

        TNull* AsNull() override;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct TNumber : public TValue<tint, tnum>
    {
        tnum number;

        void Visit(TVisitor<tint, tnum>* visitor) override;

        TNumber<tint, tnum>* AsNumber() override;

        explicit TNumber(tnum d);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct TObject : public TValue<tint, tnum>
    {
        std::map<std::string, std::unique_ptr<TValue<tint, tnum>>> object;

        void Visit(TVisitor<tint, tnum>* visitor) override;

        TObject<tint, tnum>* AsObject() override;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct TString : public TValue<tint, tnum>
    {
        std::string string;

        void Visit(TVisitor<tint, tnum>* visitor) override;

        TString<tint, tnum>* AsString() override;

        explicit TString(const std::string& s = "");
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    struct TVisitor
    {
        // will not recurse, if you want to visit the children, you have to keep calling Visit
        virtual void VisitObject(TObject<tint, tnum>* object) = 0;
        virtual void VisitArray(TArray<tint, tnum>* array) = 0;

        virtual void VisitString(TString<tint, tnum>* string) = 0;
        virtual void VisitNumber(TNumber<tint, tnum>* number) = 0;
        virtual void VisitBool(TBool<tint, tnum>* boolean) = 0;
        virtual void VisitNull(TNull<tint, tnum>* null) = 0;
        virtual void VisitInt(TInt<tint, tnum>* integer) = 0;
    };

    std::ostream& operator<<(std::ostream& s, const Location& location);
    std::ostream& operator<<(std::ostream& s, const ErrorType& type);
    std::ostream& operator<<(std::ostream& s, const Error& error);

    template <typename tint, typename tnum>
    std::ostream& operator<<(std::ostream& s, const ParseResult<tint, tnum>& result)
    {
        if (result.HasError())
        {
            s << "Errors:\n";
            for (const auto& e : result.errors)
            {
                s << "  " << e << "\n";
            }
        }
        else
        {
            s << Print(result.value.get(), PrintFlags::Json, PrettyPrint::Compact());
        }
        return s;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// templates

namespace jsonh
{
    template <typename tint, typename tnum>
    struct PrettyPrintVisitor : public TVisitor<tint, tnum>
    {
        PrettyPrint settings;
        std::ostream* stream;
        int indent = 0;

        void StreamString(const std::string& str)
        {
            *stream << '\"';
            for (char c : str)
            {
                switch (c)
                {
                case '\"':
                case '\\':
                    *stream << "\\" << c;
                    break;
                case '\b':
                    *stream << "\\b";
                    break;
                case '\f':
                    *stream << "\\f";
                    break;
                case '\n':
                    *stream << "\\n";
                    break;
                case '\r':
                    *stream << "\\r";
                    break;
                case '\t':
                    *stream << "\\t";
                    break;
                default:
                    *stream << c;
                    break;
                }
            }
            *stream << '\"';
        }

        void Indent()
        {
            for (int i = 0; i < indent; i += 1)
            {
                *stream << settings.indent;
            }
        }

        void VisitObject(TObject<tint, tnum>* object) override
        {
            *stream << '{' << settings.newline;
            indent += 1;
            for (auto o = object->object.begin(); o != object->object.end(); ++o)
            {
                Indent();
                StreamString(o->first);
                *stream << ':' << settings.space;
                o->second->Visit(this);
                if (std::next(o) != object->object.end())
                {
                    *stream << ',';
                }
                *stream << settings.newline;
            }
            indent -= 1;
            Indent();
            *stream << '}';
        }
        void VisitArray(TArray<tint, tnum>* array) override
        {
            *stream << '[' << settings.newline;
            indent += 1;
            for (auto o = array->array.begin(); o != array->array.end(); ++o)
            {
                Indent();
                (*o)->Visit(this);
                if (std::next(o) != array->array.end())
                {
                    *stream << ',';
                }
                *stream << settings.newline;
            }
            indent -= 1;
            Indent();
            *stream << ']';
        }

        void VisitString(TString<tint, tnum>* string) override
        {
            StreamString(string->string);
        }
        void VisitNumber(TNumber<tint, tnum>* number) override
        {
            // can't really detect if it is -0 or 0, should we? does it have a special value?
            // https://stackoverflow.com/questions/45795397/behaviour-of-negative-zero-0-0-in-comparison-with-positive-zero-0-0/45795465
            if (number->number == 0)
                *stream << "0.0";
            else
                *stream << number->number;
        }
        void VisitBool(TBool<tint, tnum>* boolean) override
        {
            *stream << (boolean->boolean ? "true" : "false");
        }
        void VisitNull(TNull<tint, tnum>*) override
        {
            *stream << "null";
        }
        void VisitInt(TInt<tint, tnum>* integer) override
        {
            *stream << integer->integer;
        }
    };

    template <typename tint, typename tnum>
    std::string Print(TValue<tint, tnum>* value, PrintFlags::Type, const PrettyPrint& pp)
    {
        std::ostringstream ss;
        PrettyPrintVisitor<tint, tnum> vis;
        vis.settings = pp;
        vis.stream = &ss;
        value->Visit(&vis);
        ss << pp.newline;
        return ss.str();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    void TArray<tint, tnum>::Visit(TVisitor<tint, tnum>* visitor)
    {
        visitor->VisitArray(this);
    }

    template <typename tint, typename tnum>
    TArray<tint, tnum>* TArray<tint, tnum>::AsArray()
    {
        return this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    void TBool<tint, tnum>::Visit(TVisitor<tint, tnum>* visitor)
    {
        visitor->VisitBool(this);
    }

    template <typename tint, typename tnum>
    TBool<tint, tnum>* TBool<tint, tnum>::AsBool()
    {
        return this;
    }

    template <typename tint, typename tnum>
    TBool<tint, tnum>::TBool(bool b)
        : boolean(b)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    void TInt<tint, tnum>::Visit(TVisitor<tint, tnum>* visitor)
    {
        visitor->VisitInt(this);
    }

    template <typename tint, typename tnum>
    TInt<tint, tnum>* TInt<tint, tnum>::AsInt()
    {
        return this;
    }

    template <typename tint, typename tnum>
    TInt<tint, tnum>::TInt(tint i)
        : integer(i)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    void TNull<tint, tnum>::Visit(TVisitor<tint, tnum>* visitor)
    {
        visitor->VisitNull(this);
    }

    template <typename tint, typename tnum>
    TNull<tint, tnum>* TNull<tint, tnum>::AsNull()
    {
        return this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    void TNumber<tint, tnum>::Visit(TVisitor<tint, tnum>* visitor)
    {
        visitor->VisitNumber(this);
    }

    template <typename tint, typename tnum>
    TNumber<tint, tnum>* TNumber<tint, tnum>::AsNumber()
    {
        return this;
    }

    template <typename tint, typename tnum>
    TNumber<tint, tnum>::TNumber(tnum d)
        : number(d)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    void TObject<tint, tnum>::Visit(TVisitor<tint, tnum>* visitor)
    {
        visitor->VisitObject(this);
    }

    template <typename tint, typename tnum>
    TObject<tint, tnum>* TObject<tint, tnum>::AsObject()
    {
        return this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum>
    void TString<tint, tnum>::Visit(TVisitor<tint, tnum>* visitor)
    {
        visitor->VisitString(this);
    }

    template <typename tint, typename tnum>
    TString<tint, tnum>* TString<tint, tnum>::AsString()
    {
        return this;
    }

    template <typename tint, typename tnum>
    TString<tint, tnum>::TString(const std::string& s)
        : string(s)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// detail json

namespace jsonh::detail
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // template header

    template <typename tint, typename tnum>
    void AddError(ParseResult<tint, tnum>* result, Parser* parser, ErrorType type, const std::string& err);

    template <typename tint, typename tnum>
    void AddNote(ParseResult<tint, tnum>* result, const Location& loc, const std::string& note);

    template <typename tint, typename tnum>
    std::unique_ptr<TValue<tint, tnum>> ParseValue(ParseResult<tint, tnum>* result, Parser* parser);

    template <typename tint, typename tnum>
    std::unique_ptr<TObject<tint, tnum>> ParseObject(ParseResult<tint, tnum>* result, Parser* parser);

    template <typename tint, typename tnum>
    std::unique_ptr<TArray<tint, tnum>> ParseArray(ParseResult<tint, tnum>* result, Parser* parser);

    template <typename tint, typename tnum>
    std::unique_ptr<TString<tint, tnum>> ParseString(ParseResult<tint, tnum>* result, Parser* parser);

    template <typename tint, typename tnum>
    std::unique_ptr<TValue<tint, tnum>> ParseNumber(ParseResult<tint, tnum>* result, Parser* parser);

    template <typename tint, typename tnum>
    bool ParseEscapeCode(ParseResult<tint, tnum>* result, Parser* parser, std::ostringstream& ss);

    template <typename tint, typename tnum>
    ParseResult<tint, tnum> Parse(Parser* parser);
}

namespace jsonh
{
    template <typename tint, typename tnum>
    ParseResult<tint, tnum> Parse(const std::string& str, ParseFlags::Type flags)
    {
        detail::Parser parser{str, flags};
        return detail::Parse<tint, tnum>(&parser);
    }
}

namespace jsonh::detail
{
#define EXPECT(error_type, expected_char)                                                         \
    do                                                                                            \
    {                                                                                             \
        char c = parser->Read();                                                                  \
        if (c != expected_char)                                                                   \
        {                                                                                         \
            std::stringstream expect_ss;                                                          \
            expect_ss << "Expected character " << expected_char << " but found ";                 \
            AppendChar(expect_ss, c);                                                             \
            AddError(result, parser, c ? error_type : ErrorType::UnexpectedEof, expect_ss.str()); \
            return nullptr;                                                                       \
        }                                                                                         \
    } while (false)

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // template implementation

    template <typename tint, typename tnum>
    void AddError(ParseResult<tint, tnum>* result, Parser* parser, ErrorType type, const std::string& err)
    {
        result->errors.push_back(Error{type, err, Location{parser->line, parser->column}});
        // todo: assert here instead of assigning, since the value shouldn't be set anyway...
        result->value = nullptr;
    }

    template <typename tint, typename tnum>
    void AddNote(ParseResult<tint, tnum>* result, const Location& loc, const std::string& note)
    {
        result->errors.push_back(Error{ErrorType::Note, note, loc});
    }

    template <typename tint, typename tnum>
    std::unique_ptr<TValue<tint, tnum>> ParseValue(ParseResult<tint, tnum>* result, Parser* parser)
    {
        if (parser->Peek() == '[')
        {
            return ParseArray(result, parser);
        }

        if (parser->Peek() == '{')
        {
            return ParseObject(result, parser);
        }

        if (parser->Peek() == '"')
        {
            return ParseString(result, parser);
        }

        if (
            parser->Peek(0) == 't' &&
            parser->Peek(1) == 'r' &&
            parser->Peek(2) == 'u' &&
            parser->Peek(3) == 'e')
        {
            auto ret = std::make_unique<TBool<tint, tnum>>(true);
            ret->location = parser->GetLocation();
            parser->Read();  // t
            parser->Read();  // r
            parser->Read();  // u
            parser->Read();  // e
            SkipSpaces(parser);
            return ret;
        }

        if (
            parser->Peek(0) == 'f' &&
            parser->Peek(1) == 'a' &&
            parser->Peek(2) == 'l' &&
            parser->Peek(3) == 's' &&
            parser->Peek(4) == 'e')
        {
            auto ret = std::make_unique<TBool<tint, tnum>>(false);
            ret->location = parser->GetLocation();
            parser->Read();  // f
            parser->Read();  // a
            parser->Read();  // l
            parser->Read();  // s
            parser->Read();  // e
            SkipSpaces(parser);
            return ret;
        }

        if (
            parser->Peek(0) == 'n' &&
            parser->Peek(1) == 'u' &&
            parser->Peek(2) == 'l' &&
            parser->Peek(3) == 'l')
        {
            auto ret = std::make_unique<TNull<tint, tnum>>();
            ret->location = parser->GetLocation();
            parser->Read();  // n
            parser->Read();  // u
            parser->Read();  // l
            parser->Read();  // l
            SkipSpaces(parser);
            return ret;
        }

        if (IsValidFirstDigit(parser->Peek()))
        {
            return ParseNumber(result, parser);
        }

        AddError(result, parser, ErrorType::InvalidCharacter, "Unexpected character found");
        return nullptr;
    }

    template <typename tint, typename tnum>
    std::unique_ptr<TArray<tint, tnum>> ParseArray(ParseResult<tint, tnum>* result, Parser* parser)
    {
        auto array = std::make_unique<TArray<tint, tnum>>();
        array->location = parser->GetLocation();

        EXPECT(ErrorType::InvalidCharacter, '[');
        SkipSpaces(parser);

        const auto start_of_array = parser->GetLocation();

        bool first = true;
        while (parser->HasMoreChar() && parser->Peek() != ']')
        {
            if (first)
            {
                first = false;
            }
            else
            {
                EXPECT(ErrorType::InvalidCharacter, ',');
                SkipSpaces(parser);
            }

            auto v = ParseValue(result, parser);
            if (v == nullptr)
            {
                return nullptr;
            }
            else
            {
                array->array.emplace_back(std::move(v));
            }
            SkipSpaces(parser);

            const auto next_char = parser->Peek();
            switch (next_char)
            {
            case ':':
                AddError(result, parser, ErrorType::InvalidCharacter, "Found colon instead of comma");
                return nullptr;
            case '}':
                AddError(result, parser, ErrorType::UnclosedArray, "Found }. A square bracket ] closes the array.");
                AddNote(result, start_of_array, "TArray started here");
                return nullptr;
            }
        }

        const auto end = parser->Read();
        if (end != ']')
        {
            std::ostringstream ss;
            ss << "Expected end of array but found ";
            AppendChar(ss, end);
            AddError(result, parser, ErrorType::UnclosedArray, ss.str());
            AddNote(result, start_of_array, "TArray started here");
            return nullptr;
        }
        return array;
    }

    template <typename tint, typename tnum>
    std::unique_ptr<TObject<tint, tnum>> ParseObject(ParseResult<tint, tnum>* result, Parser* parser)
    {
        EXPECT(ErrorType::InvalidCharacter, '{');
        auto object = std::make_unique<TObject<tint, tnum>>();
        object->location = parser->GetLocation();
        SkipSpaces(parser);

        bool first = true;
        while (parser->HasMoreChar() && parser->Peek() != '}')
        {
            if (first)
            {
                first = false;
            }
            else
            {
                EXPECT(ErrorType::InvalidCharacter, ',');
                SkipSpaces(parser);
            }

            auto s = ParseString(result, parser);
            if (s == nullptr)
            {
                return nullptr;
            }
            EXPECT(ErrorType::InvalidCharacter, ':');
            SkipSpaces(parser);
            auto v = ParseValue(result, parser);
            if (v == nullptr)
            {
                return nullptr;
            }
            else
            {
                if (!(parser->flags & ParseFlags::DuplicateKeysOnlyLatest))
                {
                    auto found = object->object.find(s->string);
                    if (found != object->object.end())
                    {
                        AddError(result, parser, ErrorType::DuplicateKey, "Duplicate key found " + s->string);
                        AddNote(result, found->second->location, "Previously defined here");
                        return nullptr;
                    }
                }
                object->object[s->string] = std::move(v);
            }
            SkipSpaces(parser);
        }

        EXPECT(ErrorType::InvalidCharacter, '}');
        SkipSpaces(parser);

        return object;
    }

    template <typename tint, typename tnum>
    bool ParseEscapeCode(ParseResult<tint, tnum>* result, Parser* parser, std::ostringstream& ss)
    {
#define ESCAPE(c, r)         \
    if (parser->Peek() == c) \
    {                        \
        parser->Read();      \
        ss << r;             \
    }
        ESCAPE('\"', '\"')
        else ESCAPE('\\', '\\') else ESCAPE('/', '/') else ESCAPE('b', '\b') else ESCAPE('f', '\f') else ESCAPE('n', '\n') else ESCAPE('r', '\r') else ESCAPE('t', '\t')
#undef ESCAPE
            else if (parser->Peek() == 'u' && IsHex(parser->Peek(1)) && IsHex(parser->Peek(2)) && IsHex(parser->Peek(3)) && IsHex(parser->Peek(4)))
        {
            parser->Read();                               // u
            const auto hex1 = CharToHex(parser->Read());  // hex1
            const auto hex2 = CharToHex(parser->Read());  // hex2
            const auto hex3 = CharToHex(parser->Read());  // hex3
            const auto hex4 = CharToHex(parser->Read());  // hex4
            const char val = (hex1 << 12) | (hex2 << 8) | (hex3 << 4) | hex4;
            ss << val;
        }
        else
        {
            const char escape_char = parser->Peek();
            std::ostringstream ess;
            ess << "Illegal escape sequence: ";
            AppendChar(ess, escape_char);
            AddError(result, parser, ErrorType::IllegalEscape, ess.str());
            return false;
        }

        return true;
    }

    template <typename tint, typename tnum>
    std::unique_ptr<TString<tint, tnum>> ParseString(ParseResult<tint, tnum>* result, Parser* parser)
    {
        const auto loc = parser->GetLocation();
        EXPECT(ErrorType::InvalidCharacter, '\"');

        std::ostringstream string_buffer;

        while (parser->Peek() != '\"')
        {
            const char c = parser->Read();
            if (c == 0)
            {
                AddError(result, parser, ErrorType::InvalidCharacter, "Unexpected EOF in string");
                return nullptr;
            }
            else if (c == '\\')
            {
                if (!ParseEscapeCode(result, parser, string_buffer))
                {
                    return nullptr;
                }
            }
            // is this correct? checker fail25 and fail27 seems to think so
            // but json.org says to allow any unicode character
            else if (c == '\n' || c == '\t')
            {
                std::ostringstream ss;
                ss << "the ";
                AppendChar(ss, c);
                ss << " character must be escaped";
                AddError(result, parser, ErrorType::InvalidCharacterInString, ss.str());
                return nullptr;
            }
            else
            {
                string_buffer << c;
            }
        }
        EXPECT(ErrorType::InvalidCharacter, '\"');
        SkipSpaces(parser);

        auto ret = std::make_unique<TString<tint, tnum>>(string_buffer.str());
        ret->location = loc;
        return ret;
    }

    template <typename tint, typename tnum>
    std::unique_ptr<TValue<tint, tnum>> ParseNumber(ParseResult<tint, tnum>* result, Parser* parser)
    {
        std::ostringstream o;

        const auto loc = parser->GetLocation();

        if (parser->Peek() == '-')
        {
            o << parser->Read();
        }

        if (parser->Peek() == '0')
        {
            o << parser->Read();
            if (IsDigit(parser->Peek()))
            {
                AddError(result, parser, ErrorType::InvalidNumber, "Numbers can't have leading zeroes");
                return nullptr;
            }
        }
        else
        {
            if (!IsDigit(parser->Peek()))
            {
                AddError(result, parser, ErrorType::InvalidCharacter, "Invalid first character as a number");
                return nullptr;
            }
            o << parser->Read();
            while (IsDigit(parser->Peek()))
            {
                o << parser->Read();
            }
        }

        bool is_integer = true;

        if (parser->Peek() == '.')
        {
            is_integer = false;
            o << parser->Read();
            if (!IsDigit(parser->Peek()))
            {
                AddError(result, parser, ErrorType::InvalidCharacter, "Invalid first character in a fractional number");
                return nullptr;
            }
            o << parser->Read();
            while (IsDigit(parser->Peek()))
            {
                o << parser->Read();
            }
        }

        if (parser->Peek() == 'e' || parser->Peek() == 'E')
        {
            is_integer = false;
            o << parser->Read();
            if (parser->Peek() == '+' || parser->Peek() == '-')
            {
                o << parser->Read();
            }

            if (!IsDigit(parser->Peek()))
            {
                AddError(result, parser, ErrorType::InvalidCharacter, "Invalid first character in a exponent");
                return nullptr;
            }
            o << parser->Read();
            while (IsDigit(parser->Peek()))
            {
                o << parser->Read();
            }
        }

        // todo: is this the correct way to parse?
        std::istringstream in(o.str());
        if (is_integer)
        {
            tint d;
            in >> d;
            if (in.fail())
            {
                AddError(result, parser, ErrorType::UnknownError, "Failed to parse integer: " + o.str());
                return nullptr;
            }
            auto ret = std::make_unique<TInt<tint, tnum>>(d);
            ret->location = loc;
            return ret;
        }
        else
        {
            tnum d;
            in >> d;
            // osx sometimes fails here, but parsing looks ok...?
            // if(in.fail())
            // {
            // AddError(result, parser, ErrorType::UnknownError, "Failed to parse number: " + o.str());
            // return nullptr;
            // }
            auto ret = std::make_unique<TNumber<tint, tnum>>(d);
            ret->location = loc;
            return ret;
        }
    }

    template <typename tint, typename tnum>
    ParseResult<tint, tnum> Parse(Parser* parser)
    {
        ParseResult<tint, tnum> res;
        SkipSpaces(parser);

        if (parser->Peek() == '[')
        {
            res.value = ParseArray(&res, parser);
            SkipSpaces(parser);
            if (res.value.get() != nullptr && parser->HasMoreChar())
            {
                AddError(&res, parser, ErrorType::NotEof, "Expected EOF after array");
            }
            return res;
        }
        else if (parser->Peek() == '{')
        {
            res.value = ParseObject(&res, parser);
            SkipSpaces(parser);
            if (res.value.get() != nullptr && parser->HasMoreChar())
            {
                AddError(&res, parser, ErrorType::NotEof, "Expected EOF after object");
            }
            return res;
        }
        else
        {
            AddError(&res, parser, ErrorType::InvalidCharacter, "Invalid character");
            return res;
        }
    }
}

#ifdef JSONH_IMPLEMENTATION

#include "jsonh/detail.inl"
#include "jsonh/jsonh.inl"

#endif
