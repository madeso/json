#include "jsonh/jsonh.h"

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
        {
            return true;
        }
        if (IsDigit(c))
        {
            return true;
        }
        return false;
    }

    struct PrettyPrintVisitor : public Visitor
    {
        PrintStyle settings;
        std::ostream* stream{};
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

        void VisitObject(Object* object) override
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
        void VisitArray(Array* array) override
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

        void VisitString(String* string) override
        {
            StreamString(string->string);
        }
        void VisitNumber(Number* number) override
        {
            // can't really detect if it is -0 or 0, should we? does it have a special value?
            // https://stackoverflow.com/questions/45795397/behaviour-of-negative-zero-0-0-in-comparison-with-positive-zero-0-0/45795465
            if (number->number == 0)
            {
                *stream << "0.0";
            }
            else
            {
                *stream << number->number;
            }
        }
        void VisitBool(Bool* boolean) override
        {
            *stream << (boolean->boolean ? "true" : "false");
        }
        void VisitNull(Null*) override
        {
            *stream << "null";
        }
        void VisitInt(Int* integer) override
        {
            *stream << integer->integer;
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    void AddError(ParseResult* result, Parser* parser, ErrorType type, const std::string& err);

    void AddNote(ParseResult* result, const Location& loc, const std::string& note);

    std::unique_ptr<Value> ParseValue(ParseResult* result, Parser* parser);

    std::unique_ptr<Object> ParseObject(ParseResult* result, Parser* parser);

    std::unique_ptr<Array> ParseArray(ParseResult* result, Parser* parser);

    std::unique_ptr<String> ParseString(ParseResult* result, Parser* parser);

    std::unique_ptr<Value> ParseNumber(ParseResult* result, Parser* parser);

    bool ParseEscapeCode(ParseResult* result, Parser* parser, std::ostringstream& ss);

    ParseResult Parse(Parser* parser);

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

    void AddError(ParseResult* result, Parser* parser, ErrorType type, const std::string& err)
    {
        result->errors.push_back(Error{type, err, Location{parser->line, parser->column}});
        // todo: assert here instead of assigning, since the value shouldn't be set anyway...
        result->value = nullptr;
    }

    void AddNote(ParseResult* result, const Location& loc, const std::string& note)
    {
        result->errors.push_back(Error{ErrorType::Note, note, loc});
    }

    std::unique_ptr<Value> ParseValue(ParseResult* result, Parser* parser)
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
            auto ret = std::make_unique<Bool>(true);
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
            auto ret = std::make_unique<Bool>(false);
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
            auto ret = std::make_unique<Null>();
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

    std::unique_ptr<Array> ParseArray(ParseResult* result, Parser* parser)
    {
        auto array = std::make_unique<Array>();
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
                AddNote(result, start_of_array, "Array started here");
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
            AddNote(result, start_of_array, "Array started here");
            return nullptr;
        }
        return array;
    }

    std::unique_ptr<Object> ParseObject(ParseResult* result, Parser* parser)
    {
        EXPECT(ErrorType::InvalidCharacter, '{');
        auto object = std::make_unique<Object>();
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
                if (!(parser->flags & parse_flags::DuplicateKeysOnlyLatest))
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

    std::unique_ptr<String> ParseString(ParseResult* result, Parser* parser)
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

        auto ret = std::make_unique<String>(string_buffer.str());
        ret->location = loc;
        return ret;
    }

    std::unique_ptr<Value> ParseNumber(ParseResult* result, Parser* parser)
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
            auto ret = std::make_unique<Int>(d);
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
            auto ret = std::make_unique<Number>(d);
            ret->location = loc;
            return ret;
        }
    }

    ParseResult Parse(Parser* parser)
    {
        ParseResult res;
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
            s << Print(result.value.get(), print_flags::Json, Compact);
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

    Object* Value::AsObject() { return nullptr; }
    Array* Value::AsArray() { return nullptr; }
    String* Value::AsString() { return nullptr; }
    Number* Value::AsNumber() { return nullptr; }
    Bool* Value::AsBool() { return nullptr; }
    Null* Value::AsNull() { return nullptr; }
    Int* Value::AsInt() { return nullptr; }

    std::string Print(Value* value, print_flags::Type, const PrintStyle& pp)
    {
        std::ostringstream ss;
        detail::PrettyPrintVisitor vis;
        vis.settings = pp;
        vis.stream = &ss;
        value->Visit(&vis);
        ss << pp.newline;
        return ss.str();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    void Array::Visit(Visitor* visitor)
    {
        visitor->VisitArray(this);
    }

    Array* Array::AsArray()
    {
        return this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    void Bool::Visit(Visitor* visitor)
    {
        visitor->VisitBool(this);
    }

    Bool* Bool::AsBool()
    {
        return this;
    }

    Bool::Bool(bool b)
        : boolean(b)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    void Int::Visit(Visitor* visitor)
    {
        visitor->VisitInt(this);
    }

    Int* Int::AsInt()
    {
        return this;
    }

    Int::Int(tint i)
        : integer(i)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    void Null::Visit(Visitor* visitor)
    {
        visitor->VisitNull(this);
    }

    Null* Null::AsNull()
    {
        return this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    void Number::Visit(Visitor* visitor)
    {
        visitor->VisitNumber(this);
    }

    Number* Number::AsNumber()
    {
        return this;
    }

    Number::Number(tnum d)
        : number(d)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    void Object::Visit(Visitor* visitor)
    {
        visitor->VisitObject(this);
    }

    Object* Object::AsObject()
    {
        return this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    void String::Visit(Visitor* visitor)
    {
        visitor->VisitString(this);
    }

    String* String::AsString()
    {
        return this;
    }

    String::String(const std::string& s)
        : string(s)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    ParseResult Parse(const std::string& str, parse_flags::Type flags)
    {
        detail::Parser parser{str, flags};
        return detail::Parse(&parser);
    }
}
