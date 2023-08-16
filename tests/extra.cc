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

    CHECK(Print(j.value.get(), print_flags::SkipCommas, Compact) ==
          "{\"array\":[1 2 3 4 5]\"empty\":[]\"hello\":\"world\"\"world\":\"hello\"}");

    CHECK(Print(j.value.get(), print_flags::SkipCommas, Pretty) ==
          "{\n"
          "  \"array\": [\n"
          "    1\n"
          "    2\n"
          "    3\n"
          "    4\n"
          "    5\n"
          "  ]\n"
          "  \"empty\": [\n"
          "  ]\n"
          "  \"hello\": \"world\"\n"
          "  \"world\": \"hello\"\n"
          "}\n");
}

TEST_CASE("extra-id-as-string", "[extra]")
{
    auto j = Parse(R"(
        {
            hello: world42,
            world_dog: hello-world
        }
    )",
                   parse_flags::IdentifierAsString);
    CHECK(j);
    CHECK(j.errors.empty());
}
