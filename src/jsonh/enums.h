#pragma once

#include <string>
#include <vector>

namespace jsonh
{
    using tloc = std::size_t;

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

    struct Location
    {
        Location();
        Location(tloc l, tloc c);

        tloc line;
        tloc column;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //

    struct Error
    {
        ErrorType type;
        std::string message;
        Location location;

        Error(ErrorType t, const std::string& m, const Location& l = Location());
    };

}
