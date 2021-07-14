#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#ifdef JSONH_IMPLEMENTATION
#endif

namespace jsonh
{
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

    namespace ParseFlags
    {
        enum Type
        {
            None = 0,
            DuplicateKeysOnlyLatest = 1 << 1,

            Json = None
        };
    }

    namespace PrintFlags
    {
        enum Type
        {
            None = 0,

            Json = None
        };
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tloc>
    struct Location
    {
        Location()
            : line(0)
            , column(0)
        {
        }

        Location(tloc l, tloc c)
            : line(l)
            , column(c)
        {
        }

        tloc line;
        tloc column;
    };

    template <typename tloc>
    std::ostream& operator<<(std::ostream& s, const Location<tloc>& location)
    {
        s << "(" << location.line << ", " << location.column << ")";
        return s;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tloc>
    struct Error
    {
        ErrorType type;
        std::string message;
        Location<tloc> location;

        Error(ErrorType t, const std::string& m, const Location<tloc>& l = Location<tloc>())
            : type(t)
            , message(m)
            , location(l)
        {
        }
    };

    template <typename tloc>
    std::ostream& operator<<(std::ostream& s, const Error<tloc>& error)
    {
        s << error.type << error.location << ": " << error.message;
        return s;
    }

}

namespace jsonh::detail
{
    constexpr char CharToHex(char c)
    {
        if (c >= '0' && c <= '9')
        {
            return c - '0';
        }
        else if (c >= 'a' && c <= 'f')
        {
            return 10 + (c - 'a');
        }
        else if (c >= 'A' && c <= 'F')
        {
            return 10 + (c - 'A');
        }
        else
        {
            // "Invalid hex code"
            return 0;
        }
    }

    template <typename tloc>
    struct Parser
    {
        std::string str;
        ParseFlags::Type flags;
        tloc index = 0;

        Parser(const std::string& s, ParseFlags::Type f);

        char Peek(tloc advance = 0) const;

        bool HasMoreChar() const;

        tloc line = 1;
        tloc column = 0;

        Location<tloc> GetLocation() const;

        char Read();
    };

    template <typename tloc>
    Parser<tloc>::Parser(const std::string& s, ParseFlags::Type f)
        : str(s)
        , flags(f)
    {
    }

    template <typename tloc>
    char Parser<tloc>::Peek(tloc advance) const
    {
        const tloc i = index + advance;
        if (index >= str.size())
            return 0;
        return str[i];
    }

    template <typename tloc>
    bool Parser<tloc>::HasMoreChar() const
    {
        return index < str.size();
    }

    template <typename tloc>
    Location<tloc> Parser<tloc>::GetLocation() const
    {
        return {line, column};
    }

    template <typename tloc>
    char Parser<tloc>::Read()
    {
        const char c = str[index];
        if (c == '\n')
        {
            line += 1;
            column = 0;
        }
        else
        {
            column += 1;
        }
        index += 1;
        return c;
    }

    constexpr bool IsSpace(char c)
    {
        if (c == ' ')
            return true;
        if (c == '\t')
            return true;
        if (c == '\n')
            return true;
        if (c == '\r')
            return true;
        return false;
    }

    template <typename tloc>
    void SkipSpaces(Parser<tloc>* parser)
    {
        while (IsSpace(parser->Peek()))
        {
            parser->Read();
        }
    }

    void AppendChar(std::ostream& s, char c);

    constexpr bool IsDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    constexpr bool IsHex(char c)
    {
        return IsDigit(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    constexpr bool IsValidFirstDigit(char c)
    {
        if (c == '-')
            return true;
        if (IsDigit(c))
            return true;
        return false;
    }
}

namespace jsonh
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    using default_tint = int64_t;
    using default_tnum = double;
    using default_tloc = std::size_t;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct TVisitor;

    template <typename tint, typename tnum, typename tloc>
    struct TValue;

    template <typename tint, typename tnum, typename tloc>
    struct TObject;

    template <typename tint, typename tnum, typename tloc>
    struct TArray;

    template <typename tint, typename tnum, typename tloc>
    struct TString;

    template <typename tint, typename tnum, typename tloc>
    struct TNumber;

    template <typename tint, typename tnum, typename tloc>
    struct TBool;

    template <typename tint, typename tnum, typename tloc>
    struct TNull;

    template <typename tint, typename tnum, typename tloc>
    struct TInt;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    using Visitor = TVisitor<default_tint, default_tnum, default_tloc>;
    using Value = TValue<default_tint, default_tnum, default_tloc>;
    using Object = TObject<default_tint, default_tnum, default_tloc>;
    using Array = TArray<default_tint, default_tnum, default_tloc>;
    using String = TString<default_tint, default_tnum, default_tloc>;
    using Number = TNumber<default_tint, default_tnum, default_tloc>;
    using Bool = TBool<default_tint, default_tnum, default_tloc>;
    using Null = TNull<default_tint, default_tnum, default_tloc>;
    using Int = TInt<default_tint, default_tnum, default_tloc>;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct TValue
    {
        Location<tloc> location;

        virtual ~TValue() = default;

        virtual void Visit(TVisitor<tint, tnum, tloc>* visitor) = 0;

        // only exact matches
        virtual TObject<tint, tnum, tloc>* AsObject() { return nullptr; }
        virtual TArray<tint, tnum, tloc>* AsArray() { return nullptr; }
        virtual TString<tint, tnum, tloc>* AsString() { return nullptr; }
        virtual TNumber<tint, tnum, tloc>* AsNumber() { return nullptr; }
        virtual TBool<tint, tnum, tloc>* AsBool() { return nullptr; }
        virtual TNull<tint, tnum, tloc>* AsNull() { return nullptr; }
        virtual TInt<tint, tnum, tloc>* AsInt() { return nullptr; }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct ParseResult
    {
        // contains errors if parsing failed
        std::vector<Error<tloc>> errors;

        // is non-null is parsing succeeded
        std::unique_ptr<TValue<tint, tnum, tloc>> value;

        bool HasError() const { return value == nullptr; }
        operator bool() const { return !HasError(); }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint = default_tint, typename tnum = default_tnum, typename tloc = default_tloc>
    ParseResult<tint, tnum, tloc> Parse(const std::string& str, ParseFlags::Type flags);

    struct PrintStyle
    {
        std::string_view indent;
        std::string_view space;
        std::string_view newline;
    };

    constexpr PrintStyle Pretty = PrintStyle{"  ", " ", "\n"};
    constexpr PrintStyle Compact = PrintStyle{};

    template <typename tint, typename tnum, typename tloc>
    std::string Print(TValue<tint, tnum, tloc>* value, PrintFlags::Type flags, const PrintStyle& pp);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct TArray : public TValue<tint, tnum, tloc>
    {
        std::vector<std::unique_ptr<TValue<tint, tnum, tloc>>> array;

        void Visit(TVisitor<tint, tnum, tloc>* visitor) override;

        TArray<tint, tnum, tloc>* AsArray() override;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct TBool : public TValue<tint, tnum, tloc>
    {
        bool boolean;

        void Visit(TVisitor<tint, tnum, tloc>* visitor) override;

        TBool<tint, tnum, tloc>* AsBool() override;

        explicit TBool(bool b);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct TInt : public TValue<tint, tnum, tloc>
    {
        tint integer;

        void Visit(TVisitor<tint, tnum, tloc>* visitor) override;

        TInt<tint, tnum, tloc>* AsInt() override;

        explicit TInt(tint i);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct TNull : public TValue<tint, tnum, tloc>
    {
        void Visit(TVisitor<tint, tnum, tloc>* visitor) override;

        TNull* AsNull() override;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct TNumber : public TValue<tint, tnum, tloc>
    {
        tnum number;

        void Visit(TVisitor<tint, tnum, tloc>* visitor) override;

        TNumber<tint, tnum, tloc>* AsNumber() override;

        explicit TNumber(tnum d);
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct TObject : public TValue<tint, tnum, tloc>
    {
        std::map<std::string, std::unique_ptr<TValue<tint, tnum, tloc>>> object;

        void Visit(TVisitor<tint, tnum, tloc>* visitor) override;

        TObject<tint, tnum, tloc>* AsObject() override;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct TString : public TValue<tint, tnum, tloc>
    {
        std::string string;

        void Visit(TVisitor<tint, tnum, tloc>* visitor) override;

        TString<tint, tnum, tloc>* AsString() override;

        explicit TString(const std::string& s = "");
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    struct TVisitor
    {
        // will not recurse, if you want to visit the children, you have to keep calling Visit
        virtual void VisitObject(TObject<tint, tnum, tloc>* object) = 0;
        virtual void VisitArray(TArray<tint, tnum, tloc>* array) = 0;

        virtual void VisitString(TString<tint, tnum, tloc>* string) = 0;
        virtual void VisitNumber(TNumber<tint, tnum, tloc>* number) = 0;
        virtual void VisitBool(TBool<tint, tnum, tloc>* boolean) = 0;
        virtual void VisitNull(TNull<tint, tnum, tloc>* null) = 0;
        virtual void VisitInt(TInt<tint, tnum, tloc>* integer) = 0;
    };

    template <typename tint, typename tnum, typename tloc>
    std::ostream& operator<<(std::ostream& s, const ParseResult<tint, tnum, tloc>& result)
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
            s << Print(result.value.get(), PrintFlags::Json, Compact);
        }
        return s;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// templates

namespace jsonh
{
    template <typename tint, typename tnum, typename tloc>
    struct PrettyPrintVisitor : public TVisitor<tint, tnum, tloc>
    {
        PrintStyle settings;
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

        void VisitObject(TObject<tint, tnum, tloc>* object) override
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
        void VisitArray(TArray<tint, tnum, tloc>* array) override
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

        void VisitString(TString<tint, tnum, tloc>* string) override
        {
            StreamString(string->string);
        }
        void VisitNumber(TNumber<tint, tnum, tloc>* number) override
        {
            // can't really detect if it is -0 or 0, should we? does it have a special value?
            // https://stackoverflow.com/questions/45795397/behaviour-of-negative-zero-0-0-in-comparison-with-positive-zero-0-0/45795465
            if (number->number == 0)
                *stream << "0.0";
            else
                *stream << number->number;
        }
        void VisitBool(TBool<tint, tnum, tloc>* boolean) override
        {
            *stream << (boolean->boolean ? "true" : "false");
        }
        void VisitNull(TNull<tint, tnum, tloc>*) override
        {
            *stream << "null";
        }
        void VisitInt(TInt<tint, tnum, tloc>* integer) override
        {
            *stream << integer->integer;
        }
    };

    template <typename tint, typename tnum, typename tloc>
    std::string Print(TValue<tint, tnum, tloc>* value, PrintFlags::Type, const PrintStyle& pp)
    {
        std::ostringstream ss;
        PrettyPrintVisitor<tint, tnum, tloc> vis;
        vis.settings = pp;
        vis.stream = &ss;
        value->Visit(&vis);
        ss << pp.newline;
        return ss.str();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    void TArray<tint, tnum, tloc>::Visit(TVisitor<tint, tnum, tloc>* visitor)
    {
        visitor->VisitArray(this);
    }

    template <typename tint, typename tnum, typename tloc>
    TArray<tint, tnum, tloc>* TArray<tint, tnum, tloc>::AsArray()
    {
        return this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    void TBool<tint, tnum, tloc>::Visit(TVisitor<tint, tnum, tloc>* visitor)
    {
        visitor->VisitBool(this);
    }

    template <typename tint, typename tnum, typename tloc>
    TBool<tint, tnum, tloc>* TBool<tint, tnum, tloc>::AsBool()
    {
        return this;
    }

    template <typename tint, typename tnum, typename tloc>
    TBool<tint, tnum, tloc>::TBool(bool b)
        : boolean(b)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    void TInt<tint, tnum, tloc>::Visit(TVisitor<tint, tnum, tloc>* visitor)
    {
        visitor->VisitInt(this);
    }

    template <typename tint, typename tnum, typename tloc>
    TInt<tint, tnum, tloc>* TInt<tint, tnum, tloc>::AsInt()
    {
        return this;
    }

    template <typename tint, typename tnum, typename tloc>
    TInt<tint, tnum, tloc>::TInt(tint i)
        : integer(i)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    void TNull<tint, tnum, tloc>::Visit(TVisitor<tint, tnum, tloc>* visitor)
    {
        visitor->VisitNull(this);
    }

    template <typename tint, typename tnum, typename tloc>
    TNull<tint, tnum, tloc>* TNull<tint, tnum, tloc>::AsNull()
    {
        return this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    void TNumber<tint, tnum, tloc>::Visit(TVisitor<tint, tnum, tloc>* visitor)
    {
        visitor->VisitNumber(this);
    }

    template <typename tint, typename tnum, typename tloc>
    TNumber<tint, tnum, tloc>* TNumber<tint, tnum, tloc>::AsNumber()
    {
        return this;
    }

    template <typename tint, typename tnum, typename tloc>
    TNumber<tint, tnum, tloc>::TNumber(tnum d)
        : number(d)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    void TObject<tint, tnum, tloc>::Visit(TVisitor<tint, tnum, tloc>* visitor)
    {
        visitor->VisitObject(this);
    }

    template <typename tint, typename tnum, typename tloc>
    TObject<tint, tnum, tloc>* TObject<tint, tnum, tloc>::AsObject()
    {
        return this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    template <typename tint, typename tnum, typename tloc>
    void TString<tint, tnum, tloc>::Visit(TVisitor<tint, tnum, tloc>* visitor)
    {
        visitor->VisitString(this);
    }

    template <typename tint, typename tnum, typename tloc>
    TString<tint, tnum, tloc>* TString<tint, tnum, tloc>::AsString()
    {
        return this;
    }

    template <typename tint, typename tnum, typename tloc>
    TString<tint, tnum, tloc>::TString(const std::string& s)
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

    template <typename tint, typename tnum, typename tloc>
    void AddError(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser, ErrorType type, const std::string& err);

    template <typename tint, typename tnum, typename tloc>
    void AddNote(ParseResult<tint, tnum, tloc>* result, const Location<tloc>& loc, const std::string& note);

    template <typename tint, typename tnum, typename tloc>
    std::unique_ptr<TValue<tint, tnum, tloc>> ParseValue(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser);

    template <typename tint, typename tnum, typename tloc>
    std::unique_ptr<TObject<tint, tnum, tloc>> ParseObject(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser);

    template <typename tint, typename tnum, typename tloc>
    std::unique_ptr<TArray<tint, tnum, tloc>> ParseArray(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser);

    template <typename tint, typename tnum, typename tloc>
    std::unique_ptr<TString<tint, tnum, tloc>> ParseString(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser);

    template <typename tint, typename tnum, typename tloc>
    std::unique_ptr<TValue<tint, tnum, tloc>> ParseNumber(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser);

    template <typename tint, typename tnum, typename tloc>
    bool ParseEscapeCode(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser, std::ostringstream& ss);

    template <typename tint, typename tnum, typename tloc>
    ParseResult<tint, tnum, tloc> Parse(Parser<tloc>* parser);
}

namespace jsonh
{
    template <typename tint, typename tnum, typename tloc>
    ParseResult<tint, tnum, tloc> Parse(const std::string& str, ParseFlags::Type flags)
    {
        detail::Parser<tloc> parser{str, flags};
        return detail::Parse<tint, tnum, tloc>(&parser);
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

    template <typename tint, typename tnum, typename tloc>
    void AddError(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser, ErrorType type, const std::string& err)
    {
        result->errors.push_back(Error{type, err, Location{parser->line, parser->column}});
        // todo: assert here instead of assigning, since the value shouldn't be set anyway...
        result->value = nullptr;
    }

    template <typename tint, typename tnum, typename tloc>
    void AddNote(ParseResult<tint, tnum, tloc>* result, const Location<tloc>& loc, const std::string& note)
    {
        result->errors.push_back(Error{ErrorType::Note, note, loc});
    }

    template <typename tint, typename tnum, typename tloc>
    std::unique_ptr<TValue<tint, tnum, tloc>> ParseValue(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser)
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
            auto ret = std::make_unique<TBool<tint, tnum, tloc>>(true);
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
            auto ret = std::make_unique<TBool<tint, tnum, tloc>>(false);
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
            auto ret = std::make_unique<TNull<tint, tnum, tloc>>();
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

    template <typename tint, typename tnum, typename tloc>
    std::unique_ptr<TArray<tint, tnum, tloc>> ParseArray(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser)
    {
        auto array = std::make_unique<TArray<tint, tnum, tloc>>();
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

    template <typename tint, typename tnum, typename tloc>
    std::unique_ptr<TObject<tint, tnum, tloc>> ParseObject(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser)
    {
        EXPECT(ErrorType::InvalidCharacter, '{');
        auto object = std::make_unique<TObject<tint, tnum, tloc>>();
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

    template <typename tint, typename tnum, typename tloc>
    bool ParseEscapeCode(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser, std::ostringstream& ss)
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

    template <typename tint, typename tnum, typename tloc>
    std::unique_ptr<TString<tint, tnum, tloc>> ParseString(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser)
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

        auto ret = std::make_unique<TString<tint, tnum, tloc>>(string_buffer.str());
        ret->location = loc;
        return ret;
    }

    template <typename tint, typename tnum, typename tloc>
    std::unique_ptr<TValue<tint, tnum, tloc>> ParseNumber(ParseResult<tint, tnum, tloc>* result, Parser<tloc>* parser)
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
            auto ret = std::make_unique<TInt<tint, tnum, tloc>>(d);
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
            auto ret = std::make_unique<TNumber<tint, tnum, tloc>>(d);
            ret->location = loc;
            return ret;
        }
    }

    template <typename tint, typename tnum, typename tloc>
    ParseResult<tint, tnum, tloc> Parse(Parser<tloc>* parser)
    {
        ParseResult<tint, tnum, tloc> res;
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

namespace jsonh
{
    std::ostream& operator<<(std::ostream& s, const ErrorType& type)
    {
        switch (type)
        {
        case ErrorType::Note:
            s << "Note";
            return s;
        case ErrorType::InvalidCharacter:
            s << "InvalidCharacter";
            return s;
        case ErrorType::UnclosedArray:
            s << "UnclosedArray";
            return s;
        case ErrorType::UnclosedObject:
            s << "UnclosedObject";
            return s;
        case ErrorType::NotEof:
            s << "NotEof";
            return s;
        case ErrorType::InvalidNumber:
            s << "InvalidNumber";
            return s;
        case ErrorType::IllegalEscape:
            s << "IllegalEscape";
            return s;
        case ErrorType::InvalidCharacterInString:
            s << "InvalidCharacterInString";
            return s;
        case ErrorType::UnexpectedEof:
            s << "UnexpectedEof";
            return s;
        case ErrorType::DuplicateKey:
            s << "DuplicateKey";
            return s;
        case ErrorType::UnknownError:
            s << "UnknownError";
            return s;
        default:
            s << "<unknown error>";
            return s;
        }
    }
}

namespace jsonh::detail
{
    void AppendChar(std::ostream& s, char c)
    {
        switch (c)
        {
        case '\n':
            s << "<newline>";
            break;
        case '\r':
            s << "<linefeed>";
            break;
        case 0:
            s << "<EOF>";
            break;
        case '\t':
            s << "<tab>";
            break;
        case ' ':
            s << "<space>";
            break;
        default:
            s << c << "(" << static_cast<int>(static_cast<unsigned char>(c)) << ")";
            break;
        }
    }
}

#endif
