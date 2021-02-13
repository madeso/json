#pragma once

#include <memory>
#include <vector>

#include "jsonh/value.h"

namespace jsonh
{
    struct Array : public Value
    {
        std::vector<std::shared_ptr<Value>> array;

        void Visit(Visitor* visitor) override;

        Array* AsArray() override;
    };
}
