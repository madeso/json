#include "catch.hpp"
#include "jsonh/jsonh.h"

using namespace jsonh;

// this comes from https://github.com/miloyip/nativejson-benchmark/tree/master/data/roundtrip

TEST_CASE("roundtrip01", "[roundtrip]")
{
    const std::string src = R"([null])";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE(j1);

    const auto dmp = Print(*j1.root, &j1.doc, print_flags::Json, Compact);
    REQUIRE(dmp == src);

    auto j2 = Parse(dmp, parse_flags::Json);
    REQUIRE(j2);

    CHECK(GetLocation(&j1.doc, *j1.root).line == 1);
    CHECK(GetLocation(&j2.doc, *j2.root).line == 1);
    CHECK(GetLocation(&j1.doc, *j1.root).column == 0);
    CHECK(GetLocation(&j2.doc, *j2.root).column == 0);

    auto aj1 = j1.root->AsArray(&j1.doc);
    auto aj2 = j2.root->AsArray(&j2.doc);

    REQUIRE(aj1);
    REQUIRE(aj2);

    REQUIRE(aj1->array.size() == 1);
    REQUIRE(aj2->array.size() == 1);

    auto n1 = aj1->array[0].AsNull(&j1.doc);
    auto n2 = aj2->array[0].AsNull(&j2.doc);

    REQUIRE(n1);
    REQUIRE(n2);

    CHECK(n1->location.line == 1);
    CHECK(n2->location.line == 1);
    CHECK(n1->location.column == 1);
    CHECK(n2->location.column == 1);
}

TEST_CASE("roundtrip02", "[roundtrip]")
{
    const std::string src = R"([true])";
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

    auto b1 = aj1->array[0].AsBool(&j1.doc);
    auto b2 = aj2->array[0].AsBool(&j2.doc);

    REQUIRE(b1);
    REQUIRE(b2);

    REQUIRE(b1->value);
    REQUIRE(b2->value);

    // get coverage...
    REQUIRE(aj1->array[0].AsObject(&j1.doc) == nullptr);
    REQUIRE(aj2->array[0].AsObject(&j2.doc) == nullptr);
    REQUIRE(aj1->array[0].AsArray(&j1.doc) == nullptr);
    REQUIRE(aj2->array[0].AsArray(&j2.doc) == nullptr);
    REQUIRE(aj1->array[0].AsString(&j1.doc) == nullptr);
    REQUIRE(aj2->array[0].AsString(&j2.doc) == nullptr);
    REQUIRE(aj1->array[0].AsNumber(&j1.doc) == nullptr);
    REQUIRE(aj2->array[0].AsNumber(&j2.doc) == nullptr);
    REQUIRE(aj1->array[0].AsNull(&j1.doc) == nullptr);
    REQUIRE(aj2->array[0].AsNull(&j2.doc) == nullptr);
    REQUIRE(aj1->array[0].AsInt(&j1.doc) == nullptr);
    REQUIRE(aj2->array[0].AsInt(&j2.doc) == nullptr);
}

TEST_CASE("roundtrip03", "[roundtrip]")
{
    const std::string src = R"([false])";
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

    auto b1 = aj1->array[0].AsBool(&j1.doc);
    auto b2 = aj2->array[0].AsBool(&j2.doc);

    REQUIRE(b1);
    REQUIRE(b2);

    REQUIRE_FALSE(b1->value);
    REQUIRE_FALSE(b2->value);
}

TEST_CASE("roundtrip04", "[roundtrip]")
{
    const std::string src = R"([0])";
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

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);

    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(i1->value == 0);
    REQUIRE(i2->value == 0);
}

TEST_CASE("roundtrip05", "[roundtrip]")
{
    const std::string src = R"(["foo"])";
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

    auto s1 = aj1->array[0].AsString(&j1.doc);
    auto s2 = aj2->array[0].AsString(&j2.doc);

    REQUIRE(s1);
    REQUIRE(s2);

    REQUIRE(s1->value == "foo");
    REQUIRE(s2->value == "foo");
}

TEST_CASE("roundtrip06", "[roundtrip]")
{
    const std::string src = R"([])";
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

    REQUIRE(aj1->array.size() == 0);
    REQUIRE(aj2->array.size() == 0);
}

TEST_CASE("roundtrip07", "[roundtrip]")
{
    const std::string src = R"({})";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE(j1);

    const auto dmp = Print(*j1.root, &j1.doc, print_flags::Json, Compact);
    REQUIRE(dmp == src);

    auto j2 = Parse(dmp, parse_flags::Json);
    REQUIRE(j2);

    auto oj1 = j1.root->AsObject(&j1.doc);
    auto oj2 = j2.root->AsObject(&j2.doc);

    REQUIRE(oj1);
    REQUIRE(oj2);

    REQUIRE(oj1->object.size() == 0);
    REQUIRE(oj2->object.size() == 0);
}

TEST_CASE("roundtrip08", "[roundtrip]")
{
    const std::string src = R"([0,1])";
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

    REQUIRE(aj1->array.size() == 2);
    REQUIRE(aj2->array.size() == 2);

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);
    REQUIRE(i1);
    REQUIRE(i2);
    REQUIRE(i1->value == 0);
    REQUIRE(i2->value == 0);

    i1 = aj1->array[1].AsInt(&j1.doc);
    i2 = aj2->array[1].AsInt(&j2.doc);
    REQUIRE(i1);
    REQUIRE(i2);
    REQUIRE(i1->value == 1);
    REQUIRE(i2->value == 1);
}

TEST_CASE("roundtrip09", "[roundtrip]")
{
    const std::string src = R"({"foo":"bar"})";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE(j1);

    const auto dmp = Print(*j1.root, &j1.doc, print_flags::Json, Compact);
    REQUIRE(dmp == src);

    auto j2 = Parse(dmp, parse_flags::Json);
    REQUIRE(j2);

    auto oj1 = j1.root->AsObject(&j1.doc);
    auto oj2 = j2.root->AsObject(&j2.doc);

    REQUIRE(oj1);
    REQUIRE(oj2);

    REQUIRE(oj1->object.size() == 1);
    REQUIRE(oj2->object.size() == 1);

    const auto& f1 = oj1->object["foo"];
    const auto& f2 = oj2->object["foo"];

    REQUIRE(f1);
    REQUIRE(f2);

    const auto& v1 = f1.AsString(&j1.doc);
    const auto& v2 = f2.AsString(&j2.doc);

    REQUIRE(v1);
    REQUIRE(v2);

    REQUIRE(v1->value == "bar");
    REQUIRE(v2->value == "bar");
}

TEST_CASE("roundtrip10", "[roundtrip]")
{
    const std::string src = R"({"a":null,"foo":"bar"})";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE(j1);

    const auto dmp = Print(*j1.root, &j1.doc, print_flags::Json, Compact);
    REQUIRE(dmp == src);

    auto j2 = Parse(dmp, parse_flags::Json);
    REQUIRE(j2);

    auto oj1 = j1.root->AsObject(&j1.doc);
    auto oj2 = j2.root->AsObject(&j2.doc);

    REQUIRE(oj1);
    REQUIRE(oj2);

    REQUIRE(oj1->object.size() == 2);
    REQUIRE(oj2->object.size() == 2);

    const auto& f1 = oj1->object["foo"];
    const auto& f2 = oj2->object["foo"];

    REQUIRE(f1);
    REQUIRE(f2);

    auto v1 = f1.AsString(&j1.doc);
    auto v2 = f2.AsString(&j2.doc);

    REQUIRE(v1);
    REQUIRE(v2);

    REQUIRE(v1->value == "bar");
    REQUIRE(v2->value == "bar");

    const auto& a1 = oj1->object["a"];
    const auto& a2 = oj2->object["a"];

    REQUIRE(a1);
    REQUIRE(a2);

    auto n1 = a1.AsNull(&j1.doc);
    auto n2 = a2.AsNull(&j2.doc);

    REQUIRE(n1);
    REQUIRE(n2);
}

TEST_CASE("roundtrip11", "[roundtrip]")
{
    const std::string src = R"([-1])";
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

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);

    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(i1->value == -1);
    REQUIRE(i2->value == -1);
}

TEST_CASE("roundtrip12", "[roundtrip]")
{
    const std::string src = R"([-2147483648])";
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

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);

    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(i1->value == -2147483648LL);
    REQUIRE(i2->value == -2147483648LL);
}

TEST_CASE("roundtrip13", "[roundtrip]")
{
    const std::string src = R"([-1234567890123456789])";
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

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);

    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(i1->value == -1234567890123456789);
    REQUIRE(i2->value == -1234567890123456789);
}

TEST_CASE("roundtrip14", "[roundtrip]")
{
    const auto value = GENERATE(
        std::numeric_limits<tint>::min(),
        std::numeric_limits<tint>::max());
    std::ostringstream source;
    source << "[" << value << "]";

    const std::string src = source.str();
    auto j1 = Parse(src, parse_flags::Json);

    INFO(src);
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

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);

    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(i1->value == value);
    REQUIRE(i2->value == value);
}

TEST_CASE("roundtrip15", "[roundtrip]")
{
    const std::string src = R"([1])";
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

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);

    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(i1->value == 1);
    REQUIRE(i2->value == 1);
}

TEST_CASE("roundtrip16", "[roundtrip]")
{
    const std::string src = R"([2147483647])";
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

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);

    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(i1->value == 2147483647);
    REQUIRE(i2->value == 2147483647);
}

TEST_CASE("roundtrip17", "[roundtrip]")
{
    const std::string src = R"([4294967295])";
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

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);

    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(i1->value == 4294967295);
    REQUIRE(i2->value == 4294967295);
}

TEST_CASE("roundtrip18", "[roundtrip]")
{
    const std::string src = R"([1234567890123456789])";
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

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);

    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(i1->value == 1234567890123456789);
    REQUIRE(i2->value == 1234567890123456789);
}

TEST_CASE("roundtrip19", "[roundtrip]")
{
    const std::string src = R"([9223372036854775807])";
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

    auto i1 = aj1->array[0].AsInt(&j1.doc);
    auto i2 = aj2->array[0].AsInt(&j2.doc);

    REQUIRE(i1);
    REQUIRE(i2);

    REQUIRE(i1->value == 9223372036854775807);
    REQUIRE(i2->value == 9223372036854775807);
}

TEST_CASE("roundtrip20", "[roundtrip]")
{
    const std::string src = R"([0.0])";
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

    auto n1 = aj1->array[0].AsNumber(&j1.doc);
    auto n2 = aj2->array[0].AsNumber(&j2.doc);

    REQUIRE(n1);
    REQUIRE(n2);

    REQUIRE(n1->value == Approx(0.0));
    REQUIRE(n2->value == Approx(0.0));
}

// should we support -0.0
/*
TEST_CASE("roundtrip21", "[roundtrip]")
{
  const std::string src = R"([-0.0])";
  auto j = Parse(src, parse_flags::Json);
  REQUIRE(j);
  const auto dmp = Print(j.value.get(), print_flags::Json, Compact);
  REQUIRE(dmp == src);
}
*/

TEST_CASE("roundtrip22", "[roundtrip]")
{
    const std::string src = R"([1.2345])";
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

    auto n1 = aj1->array[0].AsNumber(&j1.doc);
    auto n2 = aj2->array[0].AsNumber(&j2.doc);

    REQUIRE(n1);
    REQUIRE(n2);

    REQUIRE(n1->value == Approx(1.2345));
    REQUIRE(n2->value == Approx(1.2345));
}

TEST_CASE("roundtrip23", "[roundtrip]")
{
    const std::string src = R"([-1.2345])";
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

    auto n1 = aj1->array[0].AsNumber(&j1.doc);
    auto n2 = aj2->array[0].AsNumber(&j2.doc);

    REQUIRE(n1);
    REQUIRE(n2);

    REQUIRE(n1->value == Approx(-1.2345));
    REQUIRE(n2->value == Approx(-1.2345));
}

// fails to parse number on osx but not on linux?
TEST_CASE("roundtrip24", "[roundtrip]")
{
    const std::string src = R"([5e-324])";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE(j1);

    const auto dmp = Print(*j1.root, &j1.doc, print_flags::Json, Compact);
    // do we need to support roundtrip of doubles?
    // REQUIRE(dmp == src);

    auto j2 = Parse(dmp, parse_flags::Json);
    REQUIRE(j2);

    auto aj1 = j1.root->AsArray(&j1.doc);
    auto aj2 = j2.root->AsArray(&j2.doc);

    REQUIRE(aj1);
    REQUIRE(aj2);

    REQUIRE(aj1->array.size() == 1);
    REQUIRE(aj2->array.size() == 1);

    auto n1 = aj1->array[0].AsNumber(&j1.doc);
    auto n2 = aj2->array[0].AsNumber(&j2.doc);

    REQUIRE(n1);
    REQUIRE(n2);

    REQUIRE(n1->value == Approx(5e-324));
    REQUIRE(n2->value == Approx(5e-324));
}

// fails to parse number on osx but not on linux?
TEST_CASE("roundtrip25", "[roundtrip]")
{
    const std::string src = R"([2.225073858507201e-308])";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE(j1);

    const auto dmp = Print(*j1.root, &j1.doc, print_flags::Json, Compact);
    // do we need to support roundtrip of doubles?
    // REQUIRE(dmp == src);

    auto j2 = Parse(dmp, parse_flags::Json);
    REQUIRE(j2);

    auto aj1 = j1.root->AsArray(&j1.doc);
    auto aj2 = j2.root->AsArray(&j2.doc);

    REQUIRE(aj1);
    REQUIRE(aj2);

    REQUIRE(aj1->array.size() == 1);
    REQUIRE(aj2->array.size() == 1);

    auto n1 = aj1->array[0].AsNumber(&j1.doc);
    auto n2 = aj2->array[0].AsNumber(&j2.doc);

    REQUIRE(n1);
    REQUIRE(n2);

    REQUIRE(n1->value == Approx(2.225073858507201e-308));
    REQUIRE(n2->value == Approx(2.225073858507201e-308));
}

TEST_CASE("roundtrip26", "[roundtrip]")
{
    const std::string src = R"([2.2250738585072014e-308])";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE(j1);

    const auto dmp = Print(*j1.root, &j1.doc, print_flags::Json, Compact);
    // do we need to support roundtrip of doubles?
    // REQUIRE(dmp == src);

    auto j2 = Parse(dmp, parse_flags::Json);
    REQUIRE(j2);

    auto aj1 = j1.root->AsArray(&j1.doc);
    auto aj2 = j2.root->AsArray(&j2.doc);

    REQUIRE(aj1);
    REQUIRE(aj2);

    REQUIRE(aj1->array.size() == 1);
    REQUIRE(aj2->array.size() == 1);

    auto n1 = aj1->array[0].AsNumber(&j1.doc);
    auto n2 = aj2->array[0].AsNumber(&j2.doc);

    REQUIRE(n1);
    REQUIRE(n2);

    REQUIRE(n1->value == Approx(2.2250738585072014e-308));
    REQUIRE(n2->value == Approx(2.2250738585072014e-308));
}

TEST_CASE("roundtrip27", "[roundtrip]")
{
    const std::string src = R"([1.7976931348623157e308])";
    auto j1 = Parse(src, parse_flags::Json);
    REQUIRE(j1);

    const auto dmp = Print(*j1.root, &j1.doc, print_flags::Json, Compact);
    // same reason as above
    // REQUIRE(dmp == src);

    auto j2 = Parse(dmp, parse_flags::Json);
    REQUIRE(j2);

    auto aj1 = j1.root->AsArray(&j1.doc);
    auto aj2 = j2.root->AsArray(&j2.doc);

    REQUIRE(aj1);
    REQUIRE(aj2);

    REQUIRE(aj1->array.size() == 1);
    REQUIRE(aj2->array.size() == 1);

    auto n1 = aj1->array[0].AsNumber(&j1.doc);
    auto n2 = aj2->array[0].AsNumber(&j2.doc);

    REQUIRE(n1);
    REQUIRE(n2);

    REQUIRE(n1->value == Approx(1.7976931348623157e308));
    REQUIRE(n2->value == Approx(1.7976931348623157e308));
}
