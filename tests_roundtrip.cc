#include "catch.hpp"
#include "jsonh.h"

// this comes from https://github.com/miloyip/nativejson-benchmark/tree/master/data/roundtrip

TEST_CASE("roundtrip01", "[roundtrip]")
{
  const std::string src = R"([null])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip02", "[roundtrip]")
{
  const std::string src = R"([true])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip03", "[roundtrip]")
{
  const std::string src = R"([false])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip04", "[roundtrip]")
{
  const std::string src = R"([0])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip05", "[roundtrip]")
{
  const std::string src = R"(["foo"])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip06", "[roundtrip]")
{
  const std::string src = R"([])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip07", "[roundtrip]")
{
  const std::string src = R"({})";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip08", "[roundtrip]")
{
  const std::string src = R"([0,1])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip09", "[roundtrip]")
{
  const std::string src = R"({"foo":"bar"})";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip10", "[roundtrip]")
{
  const std::string src = R"({"a":null,"foo":"bar"})";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip11", "[roundtrip]")
{
  const std::string src = R"([-1])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip12", "[roundtrip]")
{
  const std::string src = R"([-2147483648])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip13", "[roundtrip]")
{
  const std::string src = R"([-1234567890123456789])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip14", "[roundtrip]")
{
  const std::string src = R"([-9223372036854775808])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip15", "[roundtrip]")
{
  const std::string src = R"([1])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip16", "[roundtrip]")
{
  const std::string src = R"([2147483647])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip17", "[roundtrip]")
{
  const std::string src = R"([4294967295])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip18", "[roundtrip]")
{
  const std::string src = R"([1234567890123456789])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip19", "[roundtrip]")
{
  const std::string src = R"([9223372036854775807])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip20", "[roundtrip]")
{
  const std::string src = R"([0.0])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip21", "[roundtrip]")
{
  const std::string src = R"([-0.0])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip22", "[roundtrip]")
{
  const std::string src = R"([1.2345])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip23", "[roundtrip]")
{
  const std::string src = R"([-1.2345])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip24", "[roundtrip]")
{
  const std::string src = R"([5e-324])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip25", "[roundtrip]")
{
  const std::string src = R"([2.225073858507201e-308])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip26", "[roundtrip]")
{
  const std::string src = R"([2.2250738585072014e-308])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

TEST_CASE("roundtrip27", "[roundtrip]")
{
  const std::string src = R"([1.7976931348623157e308])";
  auto j = Parse(src);
  REQUIRE(j);
  const auto dmp = ToString(j.value.get(), PrettyPrint::Compact());
  REQUIRE(dmp == src);
}

