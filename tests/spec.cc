#include "catch.hpp"
#include "jsonh/jsonh.h"

using namespace jsonh;

// this comes from the spec http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf

TEST_CASE("same-chars", "[spec]")
{
    auto j = Parse(R"(["\u002F", "\u002f", "\/", "/"])", parse_flags::Json);
    REQUIRE(j);
    REQUIRE(j.root->AsArray(&j.doc) != nullptr);
    const auto& array = j.root->AsArray(&j.doc)->array;
    REQUIRE(array.size() == 4);

    auto* a = array[0].AsString(&j.doc);
    auto* b = array[1].AsString(&j.doc);
    auto* c = array[2].AsString(&j.doc);
    auto* d = array[3].AsString(&j.doc);

    REQUIRE(a);
    REQUIRE(b);
    REQUIRE(c);
    REQUIRE(d);

    const std::string forward_slash = "/";
    CHECK(a->value == forward_slash);
    CHECK(b->value == forward_slash);
    CHECK(c->value == forward_slash);
    CHECK(d->value == forward_slash);
}
