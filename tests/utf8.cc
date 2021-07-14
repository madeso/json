#include "catch.hpp"
#include "jsonh/jsonh.h"

using namespace jsonh;

TEST_CASE("utf8", "[utf8]")
{
    const std::string src = R"(
{
  "emoji" : {
    "😊":"✨",
    "❤":"😊",
    "✨":"❤"
  },
  "cjk" : {
    "あ":"っ",
    "前":"あ",
    "っ":"前"
  },
  "swedish" : {
    "ö":"Å",
    "Ä":"ö",
    "å":"ä"
  }
}
)";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE(j1);

    const auto dmp = Print(j1.value.get(), print_flags::Json, Compact);
    auto j2 = Parse(dmp, parse_flags::Json);
    REQUIRE(j2);

    // todo: check j1 and j2
}
