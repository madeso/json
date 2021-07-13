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

    Error::Error(ErrorType t, const std::string& m, const Location& l)
        : type(t), message(m), location(l)
    {
    }

    std::ostream& operator<<(std::ostream& s, const ErrorType& type)
    {
        switch (type)
        {
#define CASE(x)        \
    case ErrorType::x: \
        s << #x;       \
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
    //

    PrettyPrint PrettyPrint::Pretty()
    {
        return {"  ", " ", "\n"};
    }

    PrettyPrint PrettyPrint::Compact()
    {
        return {};
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    // visitor
}
