#include "jsonh/jsonh.h"

#include <cassert>


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

    struct Parser
    {
        std::string str;
        parse_flags::Type flags;
        tloc index = 0;

        Parser(const std::string& s, parse_flags::Type f);

        char Peek(tloc advance = 0) const;

        [[nodiscard]] bool HasMoreChar() const;

        tloc line = 1;
        tloc column = 0;

        Location GetLocation() const;

        char Read();

        bool has_flag(const parse_flags::Type f) const
        {
            return flags & f;
        }
    };

    Parser::Parser(const std::string& s, parse_flags::Type f)
        : str(s)
        , flags(f)
    {
    }

    char Parser::Peek(tloc advance) const
    {
        const tloc i = index + advance;
        if (index >= str.size())
        {
            return 0;
        }
        return str[i];
    }

    bool Parser::HasMoreChar() const
    {
        return index < str.size();
    }

    Location Parser::GetLocation() const
    {
        return {line, column};
    }

    char Parser::Read()
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
        {
            return true;
        }
        if (c == '\t')
        {
            return true;
        }
        if (c == '\n')
        {
            return true;
        }
        if (c == '\r')
        {
            return true;
        }
        return false;
    }

    void SkipSpaces(Parser* parser)
    {
        while (
            IsSpace(parser->Peek()) ||
            (parser->has_flag(parse_flags::IgnoreAllCommas) && parser->Peek() == ','))
        {
            parser->Read();
        }
    }

    void AppendChar(std::ostream& s, char c);

    constexpr bool IsNumberChar(char c)
    {
        return c >= '0' && c <= '9';
    }

    constexpr bool IsHex(char c)
    {
        return IsNumberChar(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    constexpr bool IsIdentifierChar(char c, bool first)
    {
        if (first == false && IsNumberChar(c))
        {
            return true;
        }
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '-';
    }

    bool IsIdentifierString(const std::string& str)
    {
        bool first = true;
        for (char c : str)
        {
            if (IsIdentifierChar(c, first) == false)
            {
                return false;
            }
            first = false;
        }
        return true;
    }

    constexpr bool IsValidFirstDigit(char c)
    {
        if (c == '-')
        {
            return true;
        }
        if (IsNumberChar(c))
        {
            return true;
        }
        return false;
    }

    struct PrettyPrintVisitor
    {
        print_flags::Type flags;
        PrintStyle settings;
        std::ostream* stream{};
        int indent = 0;

        bool has_flag(const print_flags::Type f) const
        {
            return flags & f;
        }

        void Visit(const Value& val, const Document* d)
        {
            switch (val.type)
            {
            case ValueType::Object: return VisitObject(val.AsObject(d), d);
            case ValueType::Array: return VisitArray(val.AsArray(d), d);
            case ValueType::String: return VisitString(val.AsString(d));
            case ValueType::Number: return VisitNumber(val.AsNumber(d));
            case ValueType::Int: return VisitInt(val.AsInt(d));
            case ValueType::Bool: return VisitBool(val.AsBool(d));
            case ValueType::Null: return VisitNull(val.AsNull(d));
            default:
                assert(false);
            }
        }

        void StreamString(const std::string& str)
        {
            if (has_flag(print_flags::StringAsIdent) && IsIdentifierString(str))
            {
                *stream << str;
                return;
            }

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

        void VisitObject(const Object* object, const Document* doc)
        {
            *stream << '{' << settings.newline;
            indent += 1;
            for (auto o = object->object.begin(); o != object->object.end(); ++o)
            {
                Indent();
                StreamString(o->first);
                *stream << ':' << settings.space;
                Visit(o->second, doc);
                if (std::next(o) != object->object.end() && !has_flag(print_flags::SkipCommas))
                {
                    *stream << ',';
                }
                *stream << settings.newline;
            }
            indent -= 1;
            Indent();
            *stream << '}';
        }

        void VisitArray(const Array* array, const Document* doc)
        {
            *stream << '[' << settings.newline;
            indent += 1;
            for (auto o = array->array.begin(); o != array->array.end(); ++o)
            {
                Indent();
                Visit(*o, doc);
                if (std::next(o) != array->array.end())
                {
                    if (has_flag(print_flags::SkipCommas))
                    {
                        if (settings.newline == "")
                        {
                            *stream << ' ';
                        }
                    }
                    else
                    {
                        *stream << ',';
                    }
                }
                *stream << settings.newline;
            }
            indent -= 1;
            Indent();
            *stream << ']';
        }

        void VisitString(const String* string)
        {
            StreamString(string->value);
        }

        void VisitNumber(const Number* number)
        {
            // can't really detect if it is -0 or 0, should we? does it have a special value?
            // https://stackoverflow.com/questions/45795397/behaviour-of-negative-zero-0-0-in-comparison-with-positive-zero-0-0/45795465
            if (number->value == 0)
            {
                *stream << "0.0";
            }
            else
            {
                *stream << number->value;
            }
        }

        void VisitBool(const Bool* boolean)
        {
            *stream << (boolean->value ? "true" : "false");
        }

        void VisitNull(const Null*)
        {
            *stream << "null";
        }

        void VisitInt(const Int* integer)
        {
            *stream << integer->value;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    std::optional<Value> ParseValue(ParseResult* result, Parser* parser);
    std::optional<Value> ParseObject(ParseResult* result, Parser* parser);
    std::optional<Value> ParseArray(ParseResult* result, Parser* parser);
    std::optional<String> ParseString(ParseResult* result, Parser* parser);
    std::optional<String> ParseIdentifierAsString(ParseResult* result, Parser* parser);
    std::optional<Value> ParseNumber(ParseResult* result, Parser* parser);

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
            return std::nullopt;                                                                       \
        }                                                                                         \
    } while (false)

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // template implementation

    void AddError(ParseResult* result, Parser* parser, ErrorType type, const std::string& err)
    {
        result->errors.push_back(Error{type, err, Location{parser->line, parser->column}});
        // todo: assert here instead of assigning, since the value shouldn't be set anyway...
        result->doc = {};
    }

    void AddNote(ParseResult* result, const Location& loc, const std::string& note)
    {
        result->errors.push_back(Error{ErrorType::Note, note, loc});
    }

    std::optional<Value> ParseValue(ParseResult* result, Parser* parser)
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
            const auto str = ParseString(result, parser);
            if (!str)
            {
                return std::nullopt;
            }
            return result->doc.add(*str);
        }

        if (
            parser->Peek(0) == 't' &&
            parser->Peek(1) == 'r' &&
            parser->Peek(2) == 'u' &&
            parser->Peek(3) == 'e')
        {
            auto ret = Bool{parser->GetLocation() , true};
            parser->Read();  // t
            parser->Read();  // r
            parser->Read();  // u
            parser->Read();  // e
            SkipSpaces(parser);
            return result->doc.add(ret);
        }

        if (
            parser->Peek(0) == 'f' &&
            parser->Peek(1) == 'a' &&
            parser->Peek(2) == 'l' &&
            parser->Peek(3) == 's' &&
            parser->Peek(4) == 'e')
        {
            auto ret = Bool{parser->GetLocation(), false};
            parser->Read();  // f
            parser->Read();  // a
            parser->Read();  // l
            parser->Read();  // s
            parser->Read();  // e
            SkipSpaces(parser);
            return result->doc.add(ret);
        }

        if (
            parser->Peek(0) == 'n' &&
            parser->Peek(1) == 'u' &&
            parser->Peek(2) == 'l' &&
            parser->Peek(3) == 'l')
        {
            auto ret = Null{parser->GetLocation()};
            parser->Read();  // n
            parser->Read();  // u
            parser->Read();  // l
            parser->Read();  // l
            SkipSpaces(parser);
            return result->doc.add(ret);
        }

        if (IsValidFirstDigit(parser->Peek()))
        {
            return ParseNumber(result, parser);
        }

        if (parser->has_flag(parse_flags::IdentifierAsString) && IsIdentifierChar(parser->Peek(), true))
        {
            const auto str = ParseIdentifierAsString(result, parser);
            if (!str)
            {
                return std::nullopt;
            }
            return result->doc.add(*str);
        }

        std::ostringstream str;
        str << "Unexpected character found: " << parser->Peek();
        AddError(result, parser, ErrorType::InvalidCharacter, str.str());
        return std::nullopt;
    }

    std::optional<Value> ParseArray(ParseResult* result, Parser* parser)
    {
        auto array = Array{parser->GetLocation(), {}};

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
                if (!parser->has_flag(parse_flags::IgnoreAllCommas))
                {
                    EXPECT(ErrorType::InvalidCharacter, ',');
                }
                SkipSpaces(parser);
            }

            auto v = ParseValue(result, parser);
            if (!v)
            {
                return std::nullopt;
            }
            else
            {
                array.array.emplace_back(std::move(*v));
            }
            SkipSpaces(parser);

            const auto next_char = parser->Peek();
            switch (next_char)
            {
            case ':':
                AddError(result, parser, ErrorType::InvalidCharacter, "Found colon instead of comma");
                return std::nullopt;
            case '}':
                AddError(result, parser, ErrorType::UnclosedArray, "Found }. A square bracket ] closes the array.");
                AddNote(result, start_of_array, "Array started here");
                return std::nullopt;
            }
        }

        const auto end = parser->Read();
        if (end != ']')
        {
            std::ostringstream ss;
            ss << "Expected end of array but found ";
            AppendChar(ss, end);
            AddError(result, parser, ErrorType::UnclosedArray, ss.str());
            AddNote(result, start_of_array, "Array started here");
            return std::nullopt;
        }
        return result->doc.add(array);
    }

    std::optional<Value> ParseObject(ParseResult* result, Parser* parser)
    {
        EXPECT(ErrorType::InvalidCharacter, '{');
        auto object = Object{parser->GetLocation(), {}};
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
                if (!parser->has_flag(parse_flags::IgnoreAllCommas))
                {
                    EXPECT(ErrorType::InvalidCharacter, ',');
                }
                SkipSpaces(parser);
            }

            auto s =
                parser->has_flag(parse_flags::IdentifierAsString) && IsIdentifierChar(parser->Peek(), true)
                    ? ParseIdentifierAsString(result, parser)
                    : ParseString(result, parser);
            if (!s)
            {
                return std::nullopt;
            }
            EXPECT(ErrorType::InvalidCharacter, ':');
            SkipSpaces(parser);
            auto v = ParseValue(result, parser);
            if (!v)
            {
                return std::nullopt;
            }
            else
            {
                if (!parser->has_flag(parse_flags::DuplicateKeysOnlyLatest))
                {
                    auto found = object.object.find(s->value);
                    if (found != object.object.end())
                    {
                        const auto defined_at = GetLocation(&result->doc, found->second);
                        AddError(result, parser, ErrorType::DuplicateKey, "Duplicate key found " + s->value);
                        AddNote(result, defined_at, "Previously defined here");
                        return std::nullopt;
                    }
                }
                object.object[s->value] = *v;
            }
            SkipSpaces(parser);
        }

        EXPECT(ErrorType::InvalidCharacter, '}');
        SkipSpaces(parser);

        return result->doc.add(object);
    }

    bool ParseEscapeCode(ParseResult* result, Parser* parser, std::ostringstream& ss)
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

    std::optional<String> ParseString(ParseResult* result, Parser* parser)
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
                return std::nullopt;
            }
            else if (c == '\\')
            {
                if (!ParseEscapeCode(result, parser, string_buffer))
                {
                    return std::nullopt;
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
                return std::nullopt;
            }
            else
            {
                string_buffer << c;
            }
        }
        EXPECT(ErrorType::InvalidCharacter, '\"');
        SkipSpaces(parser);

        auto ret = String{loc, string_buffer.str()};
        return ret;
    }

    std::optional<String> ParseIdentifierAsString(ParseResult* result, Parser* parser)
    {
        std::ostringstream string_buffer;
        const auto loc = parser->GetLocation();
        bool first = true;

        while (IsIdentifierChar(parser->Peek(), first))
        {
            string_buffer << parser->Read();
            first = false;
        }

        // todo(Gustav): error on empty
        auto ret = String{loc, string_buffer.str()};
        return ret;
    }

    std::optional<Value> ParseNumber(ParseResult* result, Parser* parser)
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
            if (IsNumberChar(parser->Peek()))
            {
                AddError(result, parser, ErrorType::InvalidNumber, "Numbers can't have leading zeroes");
                return std::nullopt;
            }
        }
        else
        {
            if (!IsNumberChar(parser->Peek()))
            {
                AddError(result, parser, ErrorType::InvalidCharacter, "Invalid first character as a number");
                return std::nullopt;
            }
            o << parser->Read();
            while (IsNumberChar(parser->Peek()))
            {
                o << parser->Read();
            }
        }

        bool is_integer = true;

        if (parser->Peek() == '.')
        {
            is_integer = false;
            o << parser->Read();
            if (!IsNumberChar(parser->Peek()))
            {
                AddError(result, parser, ErrorType::InvalidCharacter, "Invalid first character in a fractional number");
                return std::nullopt;
            }
            o << parser->Read();
            while (IsNumberChar(parser->Peek()))
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

            if (!IsNumberChar(parser->Peek()))
            {
                AddError(result, parser, ErrorType::InvalidCharacter, "Invalid first character in a exponent");
                return std::nullopt;
            }
            o << parser->Read();
            while (IsNumberChar(parser->Peek()))
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
                return std::nullopt;
            }
            auto ret = Int{loc, d};
            return result->doc.add(ret);
        }
        else
        {
            tnum d;
            in >> d;
            // osx sometimes fails here, but parsing looks ok...?
            // if(in.fail())
            // {
            // AddError(result, parser, ErrorType::UnknownError, "Failed to parse number: " + o.str());
            // return std::nullopt;
            // }
            auto ret = Number{loc, d};
            return result->doc.add(ret);
        }
    }

    ParseResult Parse(Parser* parser)
    {
        ParseResult res;
        SkipSpaces(parser);

        if (parser->Peek() == '[')
        {
            res.root = ParseArray(&res, parser);
            SkipSpaces(parser);
            if (res.root && parser->HasMoreChar())
            {
                AddError(&res, parser, ErrorType::NotEof, "Expected EOF after array");
            }
            return res;
        }
        else if (parser->Peek() == '{')
        {
            res.root = ParseObject(&res, parser);
            SkipSpaces(parser);
            if (res.root && parser->HasMoreChar())
            {
                AddError(&res, parser, ErrorType::NotEof, "Expected EOF after object");
            }
            return res;
        }
        else
        {
            std::ostringstream ss;
            ss << "Invalid character: " << parser->Peek();
            AddError(&res, parser, ErrorType::InvalidCharacter, ss.str());
            return res;
        }
    }

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

namespace jsonh
{
    Location GetLocation(const Document* d, const Value& val)
    {
        switch (val.type)
        {
        case ValueType::Object: return val.AsObject(d)->location;
        case ValueType::Array: return val.AsArray(d)->location;
        case ValueType::String: return val.AsString(d)->location;
        case ValueType::Number: return val.AsNumber(d)->location;
        case ValueType::Int: return val.AsInt(d)->location;
        case ValueType::Bool: return val.AsBool(d)->location;
        case ValueType::Null: return val.AsNull(d)->location;
        default:
            assert(false);
            return {0, 0};
        }
    }

    Error::Error(ErrorType t, const std::string& m, const Location& l)
        : type(t)
        , message(m)
        , location(l)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    std::ostream& operator<<(std::ostream& s, const Location& location)
    {
        s << "(" << location.line << ", " << location.column << ")";
        return s;
    }

    std::ostream& operator<<(std::ostream& s, const Error& error)
    {
        s << error.type << error.location << ": " << error.message;
        return s;
    }

    std::ostream& operator<<(std::ostream& s, const ParseResult& result)
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
            s << Print(*result.root, &result.doc, print_flags::Json, Compact);
        }
        return s;
    }

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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

#define ADD(Object, objects)\
    Value Document::add(Object x)\
    {\
        const auto r = objects.size();\
        objects.emplace_back(x);\
        return {ValueType::Object, r};\
    }\
    Object* Value::As##Object(Document* doc) \
    {\
        if (type != ValueType::Object)\
        {\
            return nullptr;\
        }\
        return &doc->objects[index];\
    }\
    const Object* Value::As##Object(const Document* doc) const \
    {                                        \
        if (type != ValueType::Object)       \
        {                                    \
            return nullptr;                  \
        }                                    \
        return &doc->objects[index];         \
    }

    ADD(Object, objects)
    ADD(Array, arrays)
    ADD(String, strings)
    ADD(Number, numbers)
    ADD(Int, ints)
    ADD(Bool, bools)
    ADD(Null, nulls)
#undef ADD

    Value::Value()
        : type(ValueType::Invalid)
        , index(0)
    {
    }

    Value::Value(ValueType vt, std::size_t i)
        : type(vt)
        , index(i)
    {
        assert(vt != ValueType::Invalid);
    }

    bool Value::is_valid() const
    {
        return type != ValueType::Invalid;
    }

    Value::operator bool() const
    {
        return is_valid();
    }

    bool Value::operator!() const
    {
        return !is_valid();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    [[nodiscard]] bool ParseResult::HasError() const { return errors.empty() == false; }
    ParseResult::operator bool() const { return !HasError(); }

    std::string Print(const Value& value, const Document* doc, print_flags::Type flags, const PrintStyle& pp)
    {
        std::ostringstream ss;
        detail::PrettyPrintVisitor vis;
        vis.flags = flags;
        vis.settings = pp;
        vis.stream = &ss;
        vis.Visit(value, doc);
        ss << pp.newline;
        return ss.str();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    ParseResult Parse(const std::string& str, parse_flags::Type flags)
    {
        detail::Parser parser{str, flags};
        auto result = detail::Parse(&parser);
        if (result.errors.empty() == false)
        {
            result.root = std::nullopt;
        }
        return result;
    }
}
