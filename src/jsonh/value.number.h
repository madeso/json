#pragma once

#include "jsonh/value.h"

namespace jsonh
{
    using tnum = double;

    struct Number : public Value
    {
        tnum number;

        void Visit(Visitor* visitor) override;

        Number* AsNumber() override;

        explicit Number(tnum d);
    };
}
