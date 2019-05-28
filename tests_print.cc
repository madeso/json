#include "catch.hpp"
#include "jsonh.h"

TEST_CASE("print-one", "[print]")
{
  const std::string pretty =
    "{\n"
    "  \"prop\": 5\n"
    "}\n"
    ;
  const std::string compact = "{\"prop\":5}";

  auto root = std::make_shared<Object>();
  root->object["prop"] = std::make_shared<Int>(5);

  const auto print_compact = Print(root.get(), PrintFlags::Json, PrettyPrint::Compact());
  const auto print_pretty =  Print(root.get(), PrintFlags::Json, PrettyPrint::Pretty());

  CHECK(compact == print_compact);
  CHECK(pretty  == print_pretty);

  CHECK(Parse(print_compact, ParseFlags::Json));
  CHECK(Parse(print_pretty,  ParseFlags::Json));
}

