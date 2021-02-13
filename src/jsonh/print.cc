#include "jsonh/print.h"

#include <sstream>

#include "jsonh/value.array.h"
#include "jsonh/value.bool.h"
#include "jsonh/value.int.h"
#include "jsonh/value.null.h"
#include "jsonh/value.number.h"
#include "jsonh/value.object.h"
#include "jsonh/value.string.h"
#include "jsonh/visitor.h"

namespace jsonh
{
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
}
