#pragma once

#include "jsonh/value.h"

namespace jsonh
{
    struct Bool : public Value
    {
        bool boolean;

        void Visit(Visitor* visitor) override;

        Bool* AsBool() override;

        explicit Bool(bool b);
    };
}
