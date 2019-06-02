#include "catch.hpp"
#include "jsonh.h"

TEST_CASE("dup", "[dup]")
{
  const std::string src = R"(
{
  "a": 4, "a": 3
}
)";
  auto j1 = Parse(src, ParseFlags::Json);
  REQUIRE_FALSE(j1);
}

