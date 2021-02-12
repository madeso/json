#include <sstream>

#include "catch.hpp"

// this is ok...
TEST_CASE("osx-ok", "[osx]")
{
    std::istringstream ssa("2.2250738585072014e-308");
    double a;
    ssa >> a;
    CHECK_FALSE(ssa.fail());
    CHECK(ssa.eof());
}

// but this is not?
// fails in osx
// read looks ok in debugger/inspector
TEST_CASE("osx-fail", "[osx][!mayfail]")
{
    std::istringstream ssb("2.22507e-308");
    double b;
    ssb >> b;
    CHECK_FALSE(ssb.fail());
    CHECK(ssb.eof());
}
