#include "catch.hpp"
#include "jsonh.h"

TEST_CASE("print-object-one", "[print]")
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


TEST_CASE("print-object-two", "[print]")
{
  const std::string pretty =
    "{\n"
    "  \"cat\": 4,\n"
    "  \"dog\": 2\n"
    "}\n"
    ;
  const std::string compact = "{\"cat\":4,\"dog\":2}";

  auto root = std::make_shared<Object>();
  root->object["cat"] = std::make_shared<Int>(4);
  root->object["dog"] = std::make_shared<Int>(2);

  const auto print_compact = Print(root.get(), PrintFlags::Json, PrettyPrint::Compact());
  const auto print_pretty =  Print(root.get(), PrintFlags::Json, PrettyPrint::Pretty());

  CHECK(compact == print_compact);
  CHECK(pretty  == print_pretty);

  CHECK(Parse(print_compact, ParseFlags::Json));
  CHECK(Parse(print_pretty,  ParseFlags::Json));
}


TEST_CASE("print-array-one", "[print]")
{
  const std::string pretty =
    "[\n"
    "  5\n"
    "]\n"
    ;
  const std::string compact = "[5]";

  auto root = std::make_shared<Array>();
  root->array.push_back(std::make_shared<Int>(5));

  const auto print_compact = Print(root.get(), PrintFlags::Json, PrettyPrint::Compact());
  const auto print_pretty =  Print(root.get(), PrintFlags::Json, PrettyPrint::Pretty());

  CHECK(compact == print_compact);
  CHECK(pretty  == print_pretty);

  CHECK(Parse(print_compact, ParseFlags::Json));
  CHECK(Parse(print_pretty,  ParseFlags::Json));
}


TEST_CASE("print-array-two", "[print]")
{
  const std::string pretty =
    "[\n"
    "  4,\n"
    "  2\n"
    "]\n"
    ;
  const std::string compact = "[4,2]";

  auto root = std::make_shared<Array>();
  root->array.push_back(std::make_shared<Int>(4));
  root->array.push_back(std::make_shared<Int>(2));

  const auto print_compact = Print(root.get(), PrintFlags::Json, PrettyPrint::Compact());
  const auto print_pretty =  Print(root.get(), PrintFlags::Json, PrettyPrint::Pretty());

  CHECK(compact == print_compact);
  CHECK(pretty  == print_pretty);

  CHECK(Parse(print_compact, ParseFlags::Json));
  CHECK(Parse(print_pretty,  ParseFlags::Json));
}

