#include "jsonh/jsonh.h"

namespace jsonh
{
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
}
