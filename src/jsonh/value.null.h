#pragma once

#include "jsonh/value.h"

namespace jsonh
{
    struct Null : public Value
    {
        void Visit(Visitor* visitor) override;

        Null* AsNull() override;
    };
}
