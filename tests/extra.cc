#include "catch.hpp"
#include "jsonh/jsonh.h"

using namespace jsonh;

// tests based on http://json.org/JSON_checker/
// If the JSON_checker is working correctly, it must accept all of the pass*.json files and reject all of the fail*.json files.

TEST_CASE("extra-commas", "[extra]")
{
    auto j = Parse(R"(
        {
            "hello": "world"
            "world": "hello"
            "array": [1 2 3 4 5]
            "empty": []
        }
    )",
                   parse_flags::IgnoreAllCommas);
    CHECK(j);
    CHECK(j.errors.empty());
}
