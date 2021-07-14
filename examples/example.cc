#include <iostream>
#include <string>

// This is a basic example how to use jsonh

#include "jsonh/jsonh.h"

// jsonh only parses strings
// I'm considering to add a simple pure virtual class
// so we don't have to load the string into memory.
std::string LoadJson();

int main()
{
    // parse the json
    auto result = jsonh::Parse(LoadJson(), jsonh::ParseFlags::Json);

    // check for and print parse errors
    if (!result)
    {
        std::cerr << "Parse error:\n";
        for (const auto& err : result.errors)
        {
            const auto& loc = err.location;
            std::cerr << "file("
                      << loc.line << ":"
                      << loc.column << "): "
                      << err.message << "\n";
        }
        return 1;
    }

    // the root of a json can either be a array or a object
    // our json in this esxample needs to be a array, so we abort if it's not

    auto* array = result.value->AsArray();
    if (array == nullptr)
    {
        // one thing that makes this json libary stand out (as far as I can tell)
        // it the fact that all json object has a location so you can display
        // proper errors when parsing a json.
        // To make this example shorter, all errors hence further
        // will only return.
        const auto& loc = result.value->location;
        std::cerr << "file("
                  << loc.line << ", "
                  << loc.column << "): "
                  << "Expected array not a object.\n";
        return 2;
    }

    // cool, after all that we have our array we can loop all the items
    for (std::unique_ptr<jsonh::Value>& item : array->array)
    {
        // now this json example is a little bit cotrived...
        // if the value is a object, we print the member text
        // if the value is a integer we print that many "random" strings

        jsonh::Object* object = item->AsObject();
        if (object)
        {
            auto found = object->object.find("text");
            if (found == object->object.end())
            {
                return 3;
            }
            jsonh::String* string = found->second->AsString();
            if (string == nullptr)
            {
                return 4;
            }
            std::cout << string->string;
            continue;
        }

        // ok, I think it's time to talk about the difference between integers and numbers
        // integers are by default int64_t, like: 1, 5, and 42..
        // numbers are by default double: like 1.5, -5.3 and 4.2
        //
        // jsonh doesn't autoconvert from numbers to integers so if you need a double,
        // you should probably autoconvert yourself instead of returning a error
        jsonh::Int* integer = item->AsInt();
        if (integer)
        {
            // accept only positive numbers, greater than 0
            if (integer->integer < 1)
            {
                return 5;
            }

            static int index = 0;
            for (int i = 0; i < integer->integer; i += 1)
            {
                switch (index % 4)
                {
                case 0:
                    std::cout << " glorious";
                    break;
                case 1:
                    std::cout << " awesome";
                    break;
                case 2:
                    std::cout << " nice";
                    break;
                case 3:
                    std::cout << " cool";
                    break;
                }
                index += 1;
            }
            continue;
        }

        // ok it's neither a object nor a integer
        // so, lets display a warning and continue (sorry I lied earlier)

        // one thing that makes this json libary stand out (as far as I know)
        // it the fact that all json values has a location so you can display
        // proper errors when parsing a json.
        const auto& loc = item->location;
        std::cerr << "WARNING: file(line: "
                  << loc.line << ", column:"
                  << loc.column << "): "
                  << "Neiter a object nor a integer.\n"
                  << "was: " << Print(item.get(), jsonh::PrintFlags::Json, jsonh::Compact)
                  << "\n";
    }

    return 0;
}

std::string LoadJson()
{
    return R"(
[
  {"text": "jsonh is a"},
  3,
  {"text": " json parser,"},
  1,
  {"text": "!\n"},
  "this string will print a waning at line 8, column 2",
  {"text": "That's all folks!\n"}
]
)";
}
