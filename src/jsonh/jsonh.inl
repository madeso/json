#include <sstream>

#include "jsonh/detail.h"
#include "jsonh/jsonh.h"

namespace jsonh
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Location

    Location::Location()
        : line(0), column(0)
    {
    }

    Location::Location(tloc l, tloc c)
        : line(l), column(c)
    {
    }

    std::ostream& operator<<(std::ostream& s, const Location& location)
    {
        s << "(" << location.line << ", " << location.column << ")";
        return s;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Error

    Error::Error(Type t, const std::string& m, const Location& l)
        : type(t), message(m), location(l)
    {
    }

    std::ostream& operator<<(std::ostream& s, const Error::Type& type)
    {
        switch (type)
        {
#define CASE(x)          \
    case Error::Type::x: \
        s << #x;         \
        break
            CASE(Note);
            CASE(InvalidCharacter);
            CASE(UnclosedArray);
            CASE(UnclosedObject);
            CASE(NotEof);
            CASE(InvalidNumber);
            CASE(IllegalEscape);
            CASE(InvalidCharacterInString);
            CASE(UnexpectedEof);
            CASE(UnknownError);
#undef CASE
        default:
            s << "<unknown error>";
            break;
        }
        return s;
    }

    std::ostream& operator<<(std::ostream& s, const Error& error)
    {
        s << error.type << error.location << ": " << error.message;
        return s;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Value

    Value::~Value() {}
    Object* Value::AsObject() { return nullptr; }
    Array* Value::AsArray() { return nullptr; }
    String* Value::AsString() { return nullptr; }
    Number* Value::AsNumber() { return nullptr; }
    Bool* Value::AsBool() { return nullptr; }
    Null* Value::AsNull() { return nullptr; }
    Int* Value::AsInt() { return nullptr; }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // ParseResult

    bool ParseResult::HasError() const
    {
        return value == nullptr;
    }

    ParseResult::operator bool() const
    {
        return !HasError();
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
            s << Print(result.value.get(), PrintFlags::Json, PrettyPrint::Compact());
        }
        return s;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    ParseResult Parse(const std::string& str, ParseFlags::Type flags)
    {
        detail::Parser parser{str, flags};
        return detail::Parse(&parser);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    PrettyPrint PrettyPrint::Pretty()
    {
        return {"  ", " ", "\n"};
    }

    PrettyPrint PrettyPrint::Compact()
    {
        return {};
    }

    struct PrettyPrintVisitor : public Visitor
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
                *stream << "0.0";
            else
                *stream << number->number;
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

    std::string Print(Value* value, PrintFlags::Type, const PrettyPrint& pp)
    {
        std::ostringstream ss;
        PrettyPrintVisitor vis;
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

    // visitor
}
