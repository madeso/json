#pragma once

#include <memory>
#include <vector>

#include "jsonh/error.h"
#include "jsonh/value.h"

namespace jsonh
{
    struct ParseResult
    {
        // contains errors if parsing failed
        std::vector<Error> errors;

        // is non-null is parsing succeeded
        std::shared_ptr<Value> value;

        bool HasError() const;

        operator bool() const;
    };

    std::ostream& operator<<(std::ostream& s, const ParseResult& result);
}
