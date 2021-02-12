#include "catch.hpp"
#include "jsonh/jsonh.h"

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
  auto j1 = Parse(src, ParseFlags::Json);
  REQUIRE(j1);

  const auto dmp = Print(j1.value.get(), PrintFlags::Json, PrettyPrint::Compact());
  auto j2 = Parse(dmp, ParseFlags::Json);
  REQUIRE(j2);

  // todo: check j1 and j2
}

