#pragma once

#include <map>
#include <memory>

#include "jsonh/value.h"

namespace jsonh
{
    struct Object : public Value
    {
        std::map<std::string, std::shared_ptr<Value>> object;

        void Visit(Visitor* visitor) override;

        Object* AsObject() override;
    };

}
