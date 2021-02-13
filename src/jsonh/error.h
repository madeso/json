#pragma once

#include <string>

#include "jsonh/location.h"

namespace jsonh
{
    struct Error
    {
        enum class Type
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
        Type type;
        std::string message;
        Location location;

        Error(Type t, const std::string& m, const Location& l = Location());
    };

    std::ostream& operator<<(std::ostream& s, const Error::Type& type);
    std::ostream& operator<<(std::ostream& s, const Error& error);
}
