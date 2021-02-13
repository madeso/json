#pragma once

#include <string>

#include "jsonh/value.h"

namespace jsonh
{
    struct String : public Value
    {
        std::string string;

        void Visit(Visitor* visitor) override;

        String* AsString() override;

        explicit String(const std::string& s = "");
    };
}
