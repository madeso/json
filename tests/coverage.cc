#include "catch.hpp"
#include "jsonh/jsonh.h"

using namespace jsonh;

#if 0
TEST_CASE("cov-object-null", "[coverage]")
{
    Object v;
    CHECK(v.AsObject() == &v);
    CHECK(v.AsArray() == nullptr);
    CHECK(v.AsString() == nullptr);
    CHECK(v.AsNumber() == nullptr);
    CHECK(v.AsBool() == nullptr);
    CHECK(v.AsNull() == nullptr);
    CHECK(v.AsInt() == nullptr);
}

TEST_CASE("cov-array-null", "[coverage]")
{
    Array v;
    CHECK(v.AsObject() == nullptr);
    CHECK(v.AsArray() == &v);
    CHECK(v.AsString() == nullptr);
    CHECK(v.AsNumber() == nullptr);
    CHECK(v.AsBool() == nullptr);
    CHECK(v.AsNull() == nullptr);
    CHECK(v.AsInt() == nullptr);
}

TEST_CASE("cov-string-null", "[coverage]")
{
    String v;
    CHECK(v.AsObject() == nullptr);
    CHECK(v.AsArray() == nullptr);
    CHECK(v.AsString() == &v);
    CHECK(v.AsNumber() == nullptr);
    CHECK(v.AsBool() == nullptr);
    CHECK(v.AsNull() == nullptr);
    CHECK(v.AsInt() == nullptr);
}

TEST_CASE("cov-number-null", "[coverage]")
{
    Number v{4.2};
    CHECK(v.AsObject() == nullptr);
    CHECK(v.AsArray() == nullptr);
    CHECK(v.AsString() == nullptr);
    CHECK(v.AsNumber() == &v);
    CHECK(v.AsBool() == nullptr);
    CHECK(v.AsNull() == nullptr);
    CHECK(v.AsInt() == nullptr);
}

TEST_CASE("cov-bool-null", "[coverage]")
{
    Bool v{false};
    CHECK(v.AsObject() == nullptr);
    CHECK(v.AsArray() == nullptr);
    CHECK(v.AsString() == nullptr);
    CHECK(v.AsNumber() == nullptr);
    CHECK(v.AsBool() == &v);
    CHECK(v.AsNull() == nullptr);
    CHECK(v.AsInt() == nullptr);
}

TEST_CASE("cov-null-null", "[coverage]")
{
    Null v;
    CHECK(v.AsObject() == nullptr);
    CHECK(v.AsArray() == nullptr);
    CHECK(v.AsString() == nullptr);
    CHECK(v.AsNumber() == nullptr);
    CHECK(v.AsBool() == nullptr);
    CHECK(v.AsNull() == &v);
    CHECK(v.AsInt() == nullptr);
}

TEST_CASE("cov-int-null", "[coverage]")
{
    Int v{42};
    CHECK(v.AsObject() == nullptr);
    CHECK(v.AsArray() == nullptr);
    CHECK(v.AsString() == nullptr);
    CHECK(v.AsNumber() == nullptr);
    CHECK(v.AsBool() == nullptr);
    CHECK(v.AsNull() == nullptr);
    CHECK(v.AsInt() == &v);
}
#endif

TEST_CASE("cov-stringending-r", "[coverage]")
{
    const std::string src = R"([\r])";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE_FALSE(j1);
}

TEST_CASE("cov-stringending-n", "[coverage]")
{
    const std::string src = R"([a])";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE_FALSE(j1);
}

TEST_CASE("cov-stringending-eof", "[coverage]")
{
    const std::string src = R"([a])";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE_FALSE(j1);
}

TEST_CASE("cov-string-escapes", "[coverage]")
{
    const std::string src = "[\" \\\\ \\b \\f \\n \\r \\t \"]";
    const std::string actual = " \\ \b \f \n \r \t ";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE(j1);

    const auto dmp = Print(*j1.root, &j1.doc, print_flags::Json, Compact);
    REQUIRE(dmp == src);

    auto j2 = Parse(dmp, parse_flags::Json);
    REQUIRE(j2);

    auto aj1 = j1.root->AsArray(&j1.doc);
    auto aj2 = j2.root->AsArray(&j2.doc);

    REQUIRE(aj1);
    REQUIRE(aj2);

    REQUIRE(aj1->array.size() == 1);
    REQUIRE(aj2->array.size() == 1);

    auto n1 = aj1->array[0].AsString(&j1.doc);
    auto n2 = aj2->array[0].AsString(&j2.doc);

    REQUIRE(n1);
    REQUIRE(n2);

    CHECK(n1->value == actual);
    CHECK(n2->value == actual);
}
