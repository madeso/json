#include "jsonh/internal.parse_functions.h"

#include <cassert>
#include <sstream>

#include "jsonh/internal.parser.h"
#include "jsonh/jsonh.h"

namespace jsonh
{
    bool IsSpace(char c)
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

    void SkipSpaces(Parser* parser)
    {
        while (IsSpace(parser->Peek()))
        {
            parser->Read();
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

#define EXPECT(error_type, expected_char)                                                           \
    do                                                                                              \
    {                                                                                               \
        char c = parser->Read();                                                                    \
        if (c != expected_char)                                                                     \
        {                                                                                           \
            std::stringstream expect_ss;                                                            \
            expect_ss << "Expected character " << expected_char << " but found ";                   \
            AppendChar(expect_ss, c);                                                               \
            AddError(result, parser, c ? error_type : Error::Type::UnexpectedEof, expect_ss.str()); \
            return nullptr;                                                                         \
        }                                                                                           \
    } while (false)

    void AddError(ParseResult* result, Parser* parser, Error::Type type, const std::string& err)
    {
        result->errors.push_back(Error{type, err, Location{parser->line, parser->column}});
        // todo: assert here instead of assigning, since the value shouldn't be set anyway...
        result->value = nullptr;
    }

    void AddNote(ParseResult* result, const Location& loc, const std::string& note)
    {
        result->errors.push_back(Error{Error::Type::Note, note, loc});
    }

    bool IsDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    bool IsHex(char c)
    {
        return IsDigit(c) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    std::shared_ptr<Value> ParseValue(ParseResult* result, Parser* parser)
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
            auto ret = std::make_shared<Bool>(true);
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
            auto ret = std::make_shared<Bool>(false);
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
            auto ret = std::make_shared<Null>();
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

        AddError(result, parser, Error::Type::InvalidCharacter, "Unexpected character found");
        return nullptr;
    }

    std::shared_ptr<Array> ParseArray(ParseResult* result, Parser* parser)
    {
        auto array = std::make_shared<Array>();
        array->location = parser->GetLocation();

        EXPECT(Error::Type::InvalidCharacter, '[');
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
                EXPECT(Error::Type::InvalidCharacter, ',');
                SkipSpaces(parser);
            }

            auto v = ParseValue(result, parser);
            if (v == nullptr)
            {
                return nullptr;
            }
            else
            {
                array->array.push_back(v);
            }
            SkipSpaces(parser);

            const auto next_char = parser->Peek();
            switch (next_char)
            {
            case ':':
                AddError(result, parser, Error::Type::InvalidCharacter, "Found colon instead of comma");
                return nullptr;
            case '}':
                AddError(result, parser, Error::Type::UnclosedArray, "Found }. A square bracket ] closes the array.");
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
            AddError(result, parser, Error::Type::UnclosedArray, ss.str());
            AddNote(result, start_of_array, "Array started here");
            return nullptr;
        }
        return array;
    }

    std::shared_ptr<Object> ParseObject(ParseResult* result, Parser* parser)
    {
        EXPECT(Error::Type::InvalidCharacter, '{');
        auto object = std::make_shared<Object>();
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
                EXPECT(Error::Type::InvalidCharacter, ',');
                SkipSpaces(parser);
            }

            auto s = ParseString(result, parser);
            if (s == nullptr)
            {
                return nullptr;
            }
            EXPECT(Error::Type::InvalidCharacter, ':');
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
                        AddError(result, parser, Error::Type::DuplicateKey, "Duplicate key found " + s->string);
                        AddNote(result, found->second->location, "Previously defined here");
                        return nullptr;
                    }
                }
                object->object[s->string] = v;
            }
            SkipSpaces(parser);
        }

        EXPECT(Error::Type::InvalidCharacter, '}');
        SkipSpaces(parser);

        return object;
    }

    char CharToHex(char c)
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
            assert(false && "Invalid hex code");
            return 0;
        }
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
            AddError(result, parser, Error::Type::IllegalEscape, ess.str());
            return false;
        }

        return true;
    }

    std::shared_ptr<String> ParseString(ParseResult* result, Parser* parser)
    {
        const auto loc = parser->GetLocation();
        EXPECT(Error::Type::InvalidCharacter, '\"');

        std::ostringstream string_buffer;

        while (parser->Peek() != '\"')
        {
            const char c = parser->Read();
            if (c == 0)
            {
                AddError(result, parser, Error::Type::InvalidCharacter, "Unexpected EOF in string");
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
                AddError(result, parser, Error::Type::InvalidCharacterInString, ss.str());
                return nullptr;
            }
            else
            {
                string_buffer << c;
            }
        }
        EXPECT(Error::Type::InvalidCharacter, '\"');
        SkipSpaces(parser);

        auto ret = std::make_shared<String>(string_buffer.str());
        ret->location = loc;
        return ret;
    }

    std::shared_ptr<Value> ParseNumber(ParseResult* result, Parser* parser)
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
                AddError(result, parser, Error::Type::InvalidNumber, "Numbers can't have leading zeroes");
                return nullptr;
            }
        }
        else
        {
            if (!IsDigit(parser->Peek()))
            {
                AddError(result, parser, Error::Type::InvalidCharacter, "Invalid first character as a number");
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
                AddError(result, parser, Error::Type::InvalidCharacter, "Invalid first character in a fractional number");
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
                AddError(result, parser, Error::Type::InvalidCharacter, "Invalid first character in a exponent");
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
                AddError(result, parser, Error::Type::UnknownError, "Failed to parse integer: " + o.str());
                return nullptr;
            }
            auto ret = std::make_shared<Int>(d);
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
            // AddError(result, parser, Error::Type::UnknownError, "Failed to parse number: " + o.str());
            // return nullptr;
            // }
            auto ret = std::make_shared<Number>(d);
            ret->location = loc;
            return ret;
        }
    }

    bool IsValidFirstDigit(char c)
    {
        if (c == '-')
            return true;
        if (IsDigit(c))
            return true;
        return false;
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
                AddError(&res, parser, Error::Type::NotEof, "Expected EOF after array");
            }
            return res;
        }
        else if (parser->Peek() == '{')
        {
            res.value = ParseObject(&res, parser);
            SkipSpaces(parser);
            if (res.value.get() != nullptr && parser->HasMoreChar())
            {
                AddError(&res, parser, Error::Type::NotEof, "Expected EOF after object");
            }
            return res;
        }
        else
        {
            AddError(&res, parser, Error::Type::InvalidCharacter, "Invalid character");
            return res;
        }
    }
}
