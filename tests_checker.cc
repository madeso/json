#include "catch.hpp"
#include "jsonh.h"

// tests based on http://json.org/JSON_checker/
// If the JSON_checker is working correctly, it must accept all of the pass*.json files and reject all of the fail*.json files. 

TEST_CASE("fail1", "[checker]")
{
  auto j = Parse(R"("A JSON payload should be an object or array, not a string.")");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail2", "[checker]")
{
  auto j = Parse(R"(["Unclosed array")");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::UnclosedArray);
}

TEST_CASE("fail3", "[checker]")
{
  auto j = Parse(R"({unquoted_key: "keys must be quoted"})");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail4", "[checker]")
{
  auto j = Parse(R"(["extra comma",])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail5", "[checker]")
{
  auto j = Parse(R"(["double extra comma",,])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail6", "[checker]")
{
  auto j = Parse(R"([   , "<-- missing value"])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail7", "[checker]")
{
  auto j = Parse(R"(["Comma after the close"],)");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::NotEof);
}

TEST_CASE("fail8", "[checker]")
{
  auto j = Parse(R"(["Extra close"]])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::NotEof);
}

TEST_CASE("fail9", "[checker]")
{
  auto j = Parse(R"({"Extra comma": true,})");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail10", "[checker]")
{
  auto j = Parse(R"({"Extra value after close": true} "misplaced quoted value")");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::NotEof);
}

TEST_CASE("fail11", "[checker]")
{
  auto j = Parse(R"({"Illegal expression": 1 + 2})");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail12", "[checker]")
{
  auto j = Parse(R"({"Illegal invocation": alert()})");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail13", "[checker]")
{
  auto j = Parse(R"({"Numbers cannot have leading zeroes": 013})");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidNumber);
}

TEST_CASE("fail14", "[checker]")
{
  auto j = Parse(R"({"Numbers cannot be hex": 0x14})");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail15", "[checker]")
{
  auto j = Parse(R"(["Illegal backslash escape: \x15"])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::IllegalEscape);
}

TEST_CASE("fail16", "[checker]")
{
  auto j = Parse(R"([\naked])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail17", "[checker]")
{
  auto j = Parse(R"(["Illegal backslash escape: \017"])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::IllegalEscape);
}
/*

fail18 "depth check" is not added due to it not being in the spec
https://stackoverflow.com/questions/42116718/is-there-an-array-depth-limitation-in-json

fail18.json:
[[[[[[[[[[[[[[[[[[[["Too deep"]]]]]]]]]]]]]]]]]]]]

*/

TEST_CASE("fail19", "[checker]")
{
  auto j = Parse(R"({"Missing colon" null})");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail20", "[checker]")
{
  auto j = Parse(R"({"Double colon":: null})");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail21", "[checker]")
{
  auto j = Parse(R"({"Comma instead of colon", null})");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail22", "[checker]")
{
  auto j = Parse(R"(["Colon instead of comma": false])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail23", "[checker]")
{
  auto j = Parse(R"(["Bad value", truth])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail24", "[checker]")
{
  auto j = Parse(R"(['single quote'])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail25", "[checker]")
{
  auto j = Parse(R"(["	tab	character	in	string	"])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacterInString);
}

TEST_CASE("fail26", "[checker]")
{
  auto j = Parse(R"(["tab\   character\   in\  string\  "])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacterInString);
}

TEST_CASE("fail27", "[checker]")
{
  auto j = Parse(R"(["line
break"])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacterInString);
}

TEST_CASE("fail28", "[checker]")
{
  auto j = Parse(R"(["line\
break"])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::IllegalEscape);
}

TEST_CASE("fail29", "[checker]")
{
  auto j = Parse(R"([0e])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail30", "[checker]")
{
  auto j = Parse(R"([0e+])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail31", "[checker]")
{
  auto j = Parse(R"([0e+-1])");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

TEST_CASE("fail32", "[checker]")
{
  auto j = Parse(R"({"Comma instead if closing brace": true,)");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::UnexpectedEof);
}

TEST_CASE("fail33", "[checker]")
{
  auto j = Parse(R"(["mismatch"})");
  REQUIRE_FALSE(j);
  REQUIRE_FALSE(j.errors.empty());
  INFO(j);
  REQUIRE(j.errors[0].type == Error::Type::InvalidCharacter);
}

//////////////////////////////////////////////////////////////////////////

TEST_CASE("pass1", "[checker]")
{
  auto j = Parse(R"(
[
    "JSON Test Pattern pass1",
    {"object with 1 member":["array with 1 element"]},
    {},
    [],
    -42,
    true,
    false,
    null,
    {
        "integer": 1234567890,
        "real": -9876.543210,
        "e": 0.123456789e-12,
        "E": 1.234567890E+34,
        "":  23456789012E66,
        "zero": 0,
        "one": 1,
        "space": " ",
        "quote": "\"",
        "backslash": "\\",
        "controls": "\b\f\n\r\t",
        "slash": "/ & \/",
        "alpha": "abcdefghijklmnopqrstuvwyz",
        "ALPHA": "ABCDEFGHIJKLMNOPQRSTUVWYZ",
        "digit": "0123456789",
        "0123456789": "digit",
        "special": "`1~!@#$%^&*()_+-={':[,]}|;.</>?",
        "hex": "\u0123\u4567\u89AB\uCDEF\uabcd\uef4A",
        "true": true,
        "false": false,
        "null": null,
        "array":[  ],
        "object":{  },
        "address": "50 St. James Street",
        "url": "http://www.JSON.org/",
        "comment": "// /* <!-- --",
        "# -- --> */": " ",
        " s p a c e d " :[1,2 , 3

,

4 , 5        ,          6           ,7        ],"compact":[1,2,3,4,5,6,7],
        "jsontext": "{\"object with 1 member\":[\"array with 1 element\"]}",
        "quotes": "&#34; \u0022 %22 0x22 034 &#x22;",
        "\/\\\"\uCAFE\uBABE\uAB98\uFCDE\ubcda\uef4A\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?"
: "A key can be any string"
    },
    0.5 ,98.6
,
99.44
,

1066,
1e1,
0.1e1,
1e-1,
1e00,2e+00,2e-00
,"rosebud"]
    )");
  REQUIRE(j);
}

TEST_CASE("pass2", "[checker]")
{
  auto j = Parse(R"(
[[[[[[[[[[[[[[[[[[["Not too deep"]]]]]]]]]]]]]]]]]]]
    )");
  REQUIRE(j);
}

TEST_CASE("pass3", "[checker]")
{
  auto j = Parse(R"(
{
    "JSON Test Pattern pass3": {
        "The outermost value": "must be an object or array.",
        "In this test": "It is an object."
    }
}
    )");
  REQUIRE(j);
}

