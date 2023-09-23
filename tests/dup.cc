#include "catch.hpp"
#include "jsonh/jsonh.h"

using namespace jsonh;

TEST_CASE("dup-err", "[dup]")
{
    const std::string src = R"(
{
  "a": 4, "a": 3
}
)";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE_FALSE(j1);
}

TEST_CASE("dup-ok", "[dup]")
{
    const std::string src = R"(
{
  "a": 4, "a": 3
}
)";
    auto j1 = Parse(src, parse_flags::DuplicateKeysOnlyLatest);
    REQUIRE(j1);
    CHECK(j1.root->AsObject(&j1.doc)->object["a"].AsInt(&j1.doc)->value == 3);
}
