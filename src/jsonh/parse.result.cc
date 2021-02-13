#include "jsonh/parse.result.h"

#include "jsonh/print.h"

namespace jsonh
{
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
}
