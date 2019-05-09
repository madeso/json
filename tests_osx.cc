#include "catch.hpp"
#include "jsonh.h"

TEST_CASE("why does shorter scientific fail to read on osx?", "[crazy][!mayfail]")
{
  // this is ok...
  SECTION("ok")
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
  SECTION("failes on osx")
  {
    std::istringstream ssb("2.22507e-308");
    double b;
    ssb >> b;
    CHECK_FALSE(ssb.fail());
    CHECK(ssb.eof());
  }
}

