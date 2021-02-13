#pragma once

#include <cstdint>

#include "jsonh/value.h"

namespace jsonh
{
    using tint = int64_t;

    struct Int : public Value
    {
        tint integer;

        void Visit(Visitor* visitor) override;

        Int* AsInt() override;

        explicit Int(tint i);
    };
}
