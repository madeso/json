#include "catch.hpp"
#include "jsonh/jsonh.h"

using namespace jsonh;

TEST_CASE("print-object-one", "[print]")
{
    const std::string pretty =
        "{\n"
        "  \"prop\": 5\n"
        "}\n";
    const std::string compact = "{\"prop\":5}";

    auto doc = Document{};
    auto root = Object{};
    root.object["prop"] = doc.add(Int{{}, 5});
    const auto json = doc.add(root);

    const auto print_compact = Print(json, &doc, print_flags::Json, Compact);
    const auto print_pretty = Print(json, &doc, print_flags::Json, Pretty);

    CHECK(compact == print_compact);
    CHECK(pretty == print_pretty);

    CHECK(Parse(print_compact, parse_flags::Json));
    CHECK(Parse(print_pretty, parse_flags::Json));
}

TEST_CASE("print-object-two", "[print]")
{
    const std::string pretty =
        "{\n"
        "  \"cat\": 4,\n"
        "  \"dog\": 2\n"
        "}\n";
    const std::string compact = "{\"cat\":4,\"dog\":2}";

    auto doc = Document{};
    auto root = Object{};
    root.object["cat"] = doc.add(Int{{}, 4});
    root.object["dog"] = doc.add(Int{{}, 2});
    const auto json = doc.add(root);

    const auto print_compact = Print(json, &doc, print_flags::Json, Compact);
    const auto print_pretty = Print(json, &doc, print_flags::Json, Pretty);

    CHECK(compact == print_compact);
    CHECK(pretty == print_pretty);

    CHECK(Parse(print_compact, parse_flags::Json));
    CHECK(Parse(print_pretty, parse_flags::Json));
}

TEST_CASE("print-array-one", "[print]")
{
    const std::string pretty =
        "[\n"
        "  5\n"
        "]\n";
    const std::string compact = "[5]";

    auto doc = Document{};
    auto root = Array{};
    root.array.push_back(doc.add(Int{{}, 5}));
    const auto json = doc.add(root);

    const auto print_compact = Print(json, &doc, print_flags::Json, Compact);
    const auto print_pretty = Print(json, &doc, print_flags::Json, Pretty);

    CHECK(compact == print_compact);
    CHECK(pretty == print_pretty);

    CHECK(Parse(print_compact, parse_flags::Json));
    CHECK(Parse(print_pretty, parse_flags::Json));
}

TEST_CASE("print-array-two", "[print]")
{
    const std::string pretty =
        "[\n"
        "  4,\n"
        "  2\n"
        "]\n";
    const std::string compact = "[4,2]";

    auto doc = Document{};
    auto root = Array{};
    root.array.push_back(doc.add(Int{{}, 4}));
    root.array.push_back(doc.add(Int{{}, 2}));
    const auto json = doc.add(root);

    const auto print_compact = Print(json, &doc, print_flags::Json, Compact);
    const auto print_pretty = Print(json, &doc, print_flags::Json, Pretty);

    CHECK(compact == print_compact);
    CHECK(pretty == print_pretty);

    CHECK(Parse(print_compact, parse_flags::Json));
    CHECK(Parse(print_pretty, parse_flags::Json));
}
