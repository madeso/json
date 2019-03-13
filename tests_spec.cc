#include "catch.hpp"
#include "jsoni.h"

// this comes from the spec http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf

TEST_CASE("same-chars", "[spec]")
{
  auto j = Parse(R"(["\u002F", "\u002f", "\/", "/"])");
  REQUIRE(j);
  REQUIRE(j.array != nullptr);
  const auto& array = j.array->array;
  REQUIRE(array.size() == 4);

  auto* a = array[0]->AsString();
  auto* b = array[1]->AsString();
  auto* c = array[2]->AsString();
  auto* d = array[3]->AsString();

  REQUIRE(a);
  REQUIRE(b);
  REQUIRE(c);
  REQUIRE(d);

  const std::string forward_slash = "/";
  CHECK(a->string == forward_slash);
  CHECK(b->string == forward_slash);
  CHECK(c->string == forward_slash);
  CHECK(d->string == forward_slash);
}

// todo: add utf8 tests
// todo: add more weird tests
// add json terminal app to time the tests https://github.com/serde-rs/json-benchmark
// https://github.com/sheredom/json.h/tree/master/test

