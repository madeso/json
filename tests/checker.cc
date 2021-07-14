#include "catch.hpp"
#include "jsonh/jsonh.h"

using namespace jsonh;

// tests based on http://json.org/JSON_checker/
// If the JSON_checker is working correctly, it must accept all of the pass*.json files and reject all of the fail*.json files.

TEST_CASE("fail1", "[checker]")
{
    auto j = Parse(R"("A JSON payload should be an object or array, not a string.")", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail2", "[checker]")
{
    auto j = Parse(R"(["Unclosed array")", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::UnclosedArray);
}

TEST_CASE("fail3", "[checker]")
{
    auto j = Parse(R"({unquoted_key: "keys must be quoted"})", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail4", "[checker]")
{
    auto j = Parse(R"(["extra comma",])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail5", "[checker]")
{
    auto j = Parse(R"(["double extra comma",,])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail6", "[checker]")
{
    auto j = Parse(R"([   , "<-- missing value"])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail7", "[checker]")
{
    auto j = Parse(R"(["Comma after the close"],)", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::NotEof);
}

TEST_CASE("fail8", "[checker]")
{
    auto j = Parse(R"(["Extra close"]])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::NotEof);
}

TEST_CASE("fail9", "[checker]")
{
    auto j = Parse(R"({"Extra comma": true,})", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail10", "[checker]")
{
    auto j = Parse(R"({"Extra value after close": true} "misplaced quoted value")", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::NotEof);
}

TEST_CASE("fail11", "[checker]")
{
    auto j = Parse(R"({"Illegal expression": 1 + 2})", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail12", "[checker]")
{
    auto j = Parse(R"({"Illegal invocation": alert()})", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail13", "[checker]")
{
    auto j = Parse(R"({"Numbers cannot have leading zeroes": 013})", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidNumber);
}

TEST_CASE("fail14", "[checker]")
{
    auto j = Parse(R"({"Numbers cannot be hex": 0x14})", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail15", "[checker]")
{
    auto j = Parse(R"(["Illegal backslash escape: \x15"])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::IllegalEscape);
}

TEST_CASE("fail16", "[checker]")
{
    auto j = Parse(R"([\naked])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail17", "[checker]")
{
    auto j = Parse(R"(["Illegal backslash escape: \017"])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::IllegalEscape);
}
/*

fail18 "depth check" is not added due to it not being in the spec
https://stackoverflow.com/questions/42116718/is-there-an-array-depth-limitation-in-json

fail18.json:
[[[[[[[[[[[[[[[[[[[["Too deep"]]]]]]]]]]]]]]]]]]]]

*/

TEST_CASE("fail19", "[checker]")
{
    auto j = Parse(R"({"Missing colon" null})", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail20", "[checker]")
{
    auto j = Parse(R"({"Double colon":: null})", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail21", "[checker]")
{
    auto j = Parse(R"({"Comma instead of colon", null})", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail22", "[checker]")
{
    auto j = Parse(R"(["Colon instead of comma": false])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail23", "[checker]")
{
    auto j = Parse(R"(["Bad value", truth])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail24", "[checker]")
{
    auto j = Parse(R"(['single quote'])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail25", "[checker]")
{
    auto j = Parse(R"(["	tab	character	in	string	"])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacterInString);
}

TEST_CASE("fail26", "[checker]")
{
    auto j = Parse(R"(["tab\   character\   in\  string\  "])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::IllegalEscape);
}

TEST_CASE("fail27", "[checker]")
{
    auto j = Parse(R"(["line
break"])",
                   ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacterInString);
}

TEST_CASE("fail28", "[checker]")
{
    auto j = Parse(R"(["line\
break"])",
                   ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::IllegalEscape);
}

TEST_CASE("fail29", "[checker]")
{
    auto j = Parse(R"([0e])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail30", "[checker]")
{
    auto j = Parse(R"([0e+])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail31", "[checker]")
{
    auto j = Parse(R"([0e+-1])", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::InvalidCharacter);
}

TEST_CASE("fail32", "[checker]")
{
    auto j = Parse(R"({"Comma instead if closing brace": true,)", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::UnexpectedEof);
}

TEST_CASE("fail33", "[checker]")
{
    auto j = Parse(R"(["mismatch"})", ParseFlags::Json);
    REQUIRE_FALSE(j);
    REQUIRE_FALSE(j.errors.empty());
    INFO(j);
    REQUIRE(j.errors[0].type == ErrorType::UnclosedArray);
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
    )",
                   ParseFlags::Json);
    REQUIRE(j);
}

TEST_CASE("pass2", "[checker]")
{
    const std::string src = R"(
[[[[[[[[[[[[[[[[[[["Not too deep"]]]]]]]]]]]]]]]]]]]
    )";
    auto j1 = Parse(src, ParseFlags::Json);
    REQUIRE(j1);

    const auto dmp = Print(j1.value.get(), PrintFlags::Json, Compact);

    auto j2 = Parse(dmp, ParseFlags::Json);
    REQUIRE(j2);

    auto aj1 = j1.value->AsArray();
    auto aj2 = j2.value->AsArray();

    REQUIRE(aj1);
    REQUIRE(aj2);

    for (int i = 0; i < 18; i += 1)
    {
        REQUIRE(aj1->array.size() == 1);
        REQUIRE(aj2->array.size() == 1);

        aj1 = aj1->array[0]->AsArray();
        aj2 = aj2->array[0]->AsArray();

        REQUIRE(aj1);
        REQUIRE(aj2);
    }

    REQUIRE(aj1->array.size() == 1);
    REQUIRE(aj2->array.size() == 1);

    auto s1 = aj1->array[0]->AsString();
    auto s2 = aj2->array[0]->AsString();

    REQUIRE(s1);
    REQUIRE(s2);

    REQUIRE(s1->string == "Not too deep");
    REQUIRE(s2->string == "Not too deep");
}

TEST_CASE("pass3", "[checker]")
{
    const std::string src = R"(
{
    "JSON Test Pattern pass3": {
        "The outermost value": "must be an object or array.",
        "In this test": "It is an object."
    }
}
    )";
    auto j1 = Parse(src, ParseFlags::Json);
    REQUIRE(j1);

    const auto dmp = Print(j1.value.get(), PrintFlags::Json, Compact);
    auto j2 = Parse(dmp, ParseFlags::Json);
    REQUIRE(j2);

    auto oj1 = j1.value->AsObject();
    auto oj2 = j2.value->AsObject();

    REQUIRE(oj1);
    REQUIRE(oj2);

    REQUIRE(oj1->object.size() == 1);
    REQUIRE(oj2->object.size() == 1);

    auto c1 = oj1->object["JSON Test Pattern pass3"]->AsObject();
    auto c2 = oj2->object["JSON Test Pattern pass3"]->AsObject();

    REQUIRE(c1);
    REQUIRE(c2);

    REQUIRE(c1->object.size() == 2);
    REQUIRE(c2->object.size() == 2);

    auto sa1 = c1->object["The outermost value"]->AsString();
    auto sa2 = c2->object["The outermost value"]->AsString();

    REQUIRE(sa1);
    REQUIRE(sa2);

    REQUIRE(sa1->string == "must be an object or array.");
    REQUIRE(sa2->string == "must be an object or array.");

    auto sb1 = c1->object["In this test"]->AsString();
    auto sb2 = c2->object["In this test"]->AsString();

    REQUIRE(sb1);
    REQUIRE(sb2);

    REQUIRE(sb1->string == "It is an object.");
    REQUIRE(sb2->string == "It is an object.");
}
