#include "catch.hpp"
#include "jsonh.h"

// this comes from https://github.com/miloyip/nativejson-benchmark/tree/master/data/roundtrip

TEST_CASE("roundtrip01", "[roundtrip]")
{
  const std::string src = R"([null])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto n1 = aj1->array[0]->AsNull();
  auto n2 = aj2->array[0]->AsNull();

  REQUIRE(n1);
  REQUIRE(n2);
}

TEST_CASE("roundtrip02", "[roundtrip]")
{
  const std::string src = R"([true])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto b1 = aj1->array[0]->AsBool();
  auto b2 = aj2->array[0]->AsBool();

  REQUIRE(b1);
  REQUIRE(b2);

  REQUIRE(b1->boolean);
  REQUIRE(b2->boolean);
}

TEST_CASE("roundtrip03", "[roundtrip]")
{
  const std::string src = R"([false])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto b1 = aj1->array[0]->AsBool();
  auto b2 = aj2->array[0]->AsBool();

  REQUIRE(b1);
  REQUIRE(b2);

  REQUIRE_FALSE(b1->boolean);
  REQUIRE_FALSE(b2->boolean);
}

TEST_CASE("roundtrip04", "[roundtrip]")
{
  const std::string src = R"([0])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();

  REQUIRE(i1);
  REQUIRE(i2);

  REQUIRE(i1->integer == 0);
  REQUIRE(i2->integer == 0);
}

TEST_CASE("roundtrip05", "[roundtrip]")
{
  const std::string src = R"(["foo"])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto s1 = aj1->array[0]->AsString();
  auto s2 = aj2->array[0]->AsString();

  REQUIRE(s1);
  REQUIRE(s2);

  REQUIRE(s1->string == "foo");
  REQUIRE(s2->string == "foo");
}

TEST_CASE("roundtrip06", "[roundtrip]")
{
  const std::string src = R"([])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 0);
  REQUIRE(aj2->array.size() == 0);
}

TEST_CASE("roundtrip07", "[roundtrip]")
{
  const std::string src = R"({})";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);


  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto oj1 = j1.value->AsObject();
  auto oj2 = j2.value->AsObject();

  REQUIRE(oj1);
  REQUIRE(oj2);

  REQUIRE(oj1->object.size() == 0);
  REQUIRE(oj2->object.size() == 0);
}

TEST_CASE("roundtrip08", "[roundtrip]")
{
  const std::string src = R"([0,1])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 2);
  REQUIRE(aj2->array.size() == 2);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();
  REQUIRE(i1);
  REQUIRE(i2);
  REQUIRE(i1->integer == 0);
  REQUIRE(i2->integer == 0);

  i1 = aj1->array[1]->AsInt();
  i2 = aj2->array[1]->AsInt();
  REQUIRE(i1);
  REQUIRE(i2);
  REQUIRE(i1->integer == 1);
  REQUIRE(i2->integer == 1);
}

TEST_CASE("roundtrip09", "[roundtrip]")
{
  const std::string src = R"({"foo":"bar"})";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto oj1 = j1.value->AsObject();
  auto oj2 = j2.value->AsObject();

  REQUIRE(oj1);
  REQUIRE(oj2);

  REQUIRE(oj1->object.size() == 1);
  REQUIRE(oj2->object.size() == 1);

  auto f1 = oj1->object["foo"];
  auto f2 = oj2->object["foo"];

  REQUIRE(f1);
  REQUIRE(f2);

  auto v1 = f1->AsString();
  auto v2 = f2->AsString();

  REQUIRE(v1);
  REQUIRE(v2);

  REQUIRE(v1->string == "bar");
  REQUIRE(v2->string == "bar");
}

TEST_CASE("roundtrip10", "[roundtrip]")
{
  const std::string src = R"({"a":null,"foo":"bar"})";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto oj1 = j1.value->AsObject();
  auto oj2 = j2.value->AsObject();

  REQUIRE(oj1);
  REQUIRE(oj2);

  REQUIRE(oj1->object.size() == 2);
  REQUIRE(oj2->object.size() == 2);

  auto f1 = oj1->object["foo"];
  auto f2 = oj2->object["foo"];

  REQUIRE(f1);
  REQUIRE(f2);

  auto v1 = f1->AsString();
  auto v2 = f2->AsString();

  REQUIRE(v1);
  REQUIRE(v2);

  REQUIRE(v1->string == "bar");
  REQUIRE(v2->string == "bar");

  auto a1 = oj1->object["a"];
  auto a2 = oj2->object["a"];

  REQUIRE(a1);
  REQUIRE(a2);

  auto n1 = a1->AsNull();
  auto n2 = a2->AsNull();

  REQUIRE(n1);
  REQUIRE(n2);
}

TEST_CASE("roundtrip11", "[roundtrip]")
{
  const std::string src = R"([-1])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();

  REQUIRE(i1);
  REQUIRE(i2);

  REQUIRE(i1->integer == -1);
  REQUIRE(i2->integer == -1);
}

TEST_CASE("roundtrip12", "[roundtrip]")
{
  const std::string src = R"([-2147483648])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();

  REQUIRE(i1);
  REQUIRE(i2);

  REQUIRE(i1->integer == -2147483648LL);
  REQUIRE(i2->integer == -2147483648LL);
}

TEST_CASE("roundtrip13", "[roundtrip]")
{
  const std::string src = R"([-1234567890123456789])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();

  REQUIRE(i1);
  REQUIRE(i2);

  REQUIRE(i1->integer == -1234567890123456789);
  REQUIRE(i2->integer == -1234567890123456789);
}

TEST_CASE("roundtrip14", "[roundtrip]")
{
  const std::string src = R"([-9223372036854775808])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();

  REQUIRE(i1);
  REQUIRE(i2);

  // osx gets a compiler warning here...
  // this should be fine since this is the lowest a signed 64bit can go
  // use INT64_C() macro?
  REQUIRE(i1->integer == -9223372036854775808LL);
  REQUIRE(i2->integer == -9223372036854775808LL);
}

TEST_CASE("roundtrip15", "[roundtrip]")
{
  const std::string src = R"([1])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();

  REQUIRE(i1);
  REQUIRE(i2);

  REQUIRE(i1->integer == 1);
  REQUIRE(i2->integer == 1);
}

TEST_CASE("roundtrip16", "[roundtrip]")
{
  const std::string src = R"([2147483647])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();

  REQUIRE(i1);
  REQUIRE(i2);

  REQUIRE(i1->integer == 2147483647);
  REQUIRE(i2->integer == 2147483647);
}

TEST_CASE("roundtrip17", "[roundtrip]")
{
  const std::string src = R"([4294967295])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();

  REQUIRE(i1);
  REQUIRE(i2);

  REQUIRE(i1->integer == 4294967295);
  REQUIRE(i2->integer == 4294967295);
}

TEST_CASE("roundtrip18", "[roundtrip]")
{
  const std::string src = R"([1234567890123456789])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();

  REQUIRE(i1);
  REQUIRE(i2);

  REQUIRE(i1->integer == 1234567890123456789);
  REQUIRE(i2->integer == 1234567890123456789);
}

TEST_CASE("roundtrip19", "[roundtrip]")
{
  const std::string src = R"([9223372036854775807])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto i1 = aj1->array[0]->AsInt();
  auto i2 = aj2->array[0]->AsInt();

  REQUIRE(i1);
  REQUIRE(i2);

  REQUIRE(i1->integer == 9223372036854775807);
  REQUIRE(i2->integer == 9223372036854775807);
}

TEST_CASE("roundtrip20", "[roundtrip]")
{
  const std::string src = R"([0.0])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto n1 = aj1->array[0]->AsNumber();
  auto n2 = aj2->array[0]->AsNumber();

  REQUIRE(n1);
  REQUIRE(n2);

  REQUIRE(n1->number == Approx(0.0));
  REQUIRE(n2->number == Approx(0.0));
}

// should we support -0.0
/*
TEST_CASE("roundtrip21", "[roundtrip]")
{
  const std::string src = R"([-0.0])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}
*/

TEST_CASE("roundtrip22", "[roundtrip]")
{
  const std::string src = R"([1.2345])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto n1 = aj1->array[0]->AsNumber();
  auto n2 = aj2->array[0]->AsNumber();

  REQUIRE(n1);
  REQUIRE(n2);

  REQUIRE(n1->number == Approx(1.2345));
  REQUIRE(n2->number == Approx(1.2345));
}

TEST_CASE("roundtrip23", "[roundtrip]")
{
  const std::string src = R"([-1.2345])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto n1 = aj1->array[0]->AsNumber();
  auto n2 = aj2->array[0]->AsNumber();

  REQUIRE(n1);
  REQUIRE(n2);

  REQUIRE(n1->number == Approx(-1.2345));
  REQUIRE(n2->number == Approx(-1.2345));
}

// fails to parse number on osx but not on linux?
TEST_CASE("roundtrip24", "[roundtrip]")
{
  const std::string src = R"([5e-324])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  // do we need to support roundtrip of doubles?
  // REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto n1 = aj1->array[0]->AsNumber();
  auto n2 = aj2->array[0]->AsNumber();

  REQUIRE(n1);
  REQUIRE(n2);

  REQUIRE(n1->number == Approx(5e-324));
  REQUIRE(n2->number == Approx(5e-324));
}

// fails to parse number on osx but not on linux?
TEST_CASE("roundtrip25", "[roundtrip]")
{
  const std::string src = R"([2.225073858507201e-308])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  // do we need to support roundtrip of doubles?
  // REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto n1 = aj1->array[0]->AsNumber();
  auto n2 = aj2->array[0]->AsNumber();

  REQUIRE(n1);
  REQUIRE(n2);

  REQUIRE(n1->number == Approx(2.225073858507201e-308));
  REQUIRE(n2->number == Approx(2.225073858507201e-308));
}

TEST_CASE("roundtrip26", "[roundtrip]")
{
  const std::string src = R"([2.2250738585072014e-308])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  // do we need to support roundtrip of doubles?
  // REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto n1 = aj1->array[0]->AsNumber();
  auto n2 = aj2->array[0]->AsNumber();

  REQUIRE(n1);
  REQUIRE(n2);

  REQUIRE(n1->number == Approx(2.2250738585072014e-308));
  REQUIRE(n2->number == Approx(2.2250738585072014e-308));
}

TEST_CASE("roundtrip27", "[roundtrip]")
{
  const std::string src = R"([1.7976931348623157e308])";
  auto j1 = Parse(src);
  REQUIRE(j1);

  const auto dmp = ToString(j1.value.get(), PrettyPrint::Compact());
  // same reason as above
  // REQUIRE(dmp == src);

  auto j2 = Parse(dmp);
  REQUIRE(j2);

  auto aj1 = j1.value->AsArray();
  auto aj2 = j2.value->AsArray();

  REQUIRE(aj1);
  REQUIRE(aj2);

  REQUIRE(aj1->array.size() == 1);
  REQUIRE(aj2->array.size() == 1);

  auto n1 = aj1->array[0]->AsNumber();
  auto n2 = aj2->array[0]->AsNumber();

  REQUIRE(n1);
  REQUIRE(n2);

  REQUIRE(n1->number == Approx(1.7976931348623157e308));
  REQUIRE(n2->number == Approx(1.7976931348623157e308));
}

