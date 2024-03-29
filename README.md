# JSON parser for files written by humans

[![windows](https://github.com/madeso/json/workflows/windows/badge.svg?branch=master)](https://github.com/madeso/json/actions?query=workflow%3Awindows)
[![linux](https://github.com/madeso/json/workflows/linux/badge.svg?branch=master)](https://github.com/madeso/json/actions?query=workflow%3Alinux)
[![macos](https://github.com/madeso/json/workflows/macos/badge.svg?branch=master)](https://github.com/madeso/json/actions?query=workflow%3Amacos)
[![Coverage Status](https://coveralls.io/repos/github/madeso/json/badge.svg?branch=master)](https://coveralls.io/github/madeso/json?branch=master)

JSON parser for files written by humans, or _jsonh_ for short, is designed to be a simple cross-platform input/output library to read [JSON](https://json.org/) and JSON-like files written by humans into a DOM like structure.

**What does simple input/output library mean?** It means there are no extra functions to create json making the library more complex. There is also no custom struct to json functions. If this is not what you are looking for, perhaps have a look at [JSON for Modern C++](https://github.com/nlohmann/json) instead.

**What does "written by humans" mean?** It means that given a choice between helpful error messages/syntax and performance, helpfulness will always win. This means that for every json value, there is a corresponding line/column entry so that when you search in your database for the user entered value and don't find it you can let the user know where in the json the value was requested. If this is not what you are looking for, perhaps have a look at [rapidjson](https://github.com/Tencent/rapidjson) instead.

**What does cross platform mean?** It means that currently each commit is built and tested with both gcc and clang(c++14) on linux and osx and visual studio 2017 on windows. It may work on others setups, but those are not tested (ports are welcome!).

## How to use

1. Add jsonh to your project, either as a git submodule or the released source.

```sh
git submodule add https://github.com/madeso/json.git
```

2. Let cmake know about it and add it to your target

```cmake
add_subdirectory(json)

target_link_libraries(my_cool_project
    PUBLIC jsonh
)
```

3. Parse and inspect some json data

```cpp
#include "jsonh/jsonh.h"
using namespace jsonh;

// 3a. Parse string into DOM structure
ParseResult j = Parse(json_string, parse_flags::Json);

// 3b. Check for errors
if(!j) { std::cerr << j; return 0; }
// or
if(j.HasErrors()) { std::cerr << j; return 0; };
// or
if(!j.errors.empty()) { std::cerr << j; return 0; };
// or
if(j.value == nullptr) { std::cerr << j; return 0; };

// 3c. Use j.value
std::unique_ptr<Value> root = j.value;

// the root of a json can eiter be a array or a object.
Array* arr = root->AsArray();
Object* obj = root->AsObject();

// Both functions return a nullptr if the root isn't array or a object.

// Array::array is a std::vector of shared_ptr to Value
//
// struct Array : public Value
// {
//   std::vector<std::unique_ptr<Value>> array;
// };


// Object::object is a std::map of std::string to shared_ptr to Value
//
// struct Object : public Value
// {
//   std::map<std::string, std::unique_ptr<Value>> object;
// };

```

Other types of values are

-   `Null` (`AsNull()`)
-   `Bool` (`AsBool()`, `Bool::boolean`)
-   `Int` (`AsInt()`, `Int::integer`)
-   `Number` (`AsNumber()`, `Number::number`)
-   `String` (`AsString()`, `String::string`)

Numbers in jsonh are either represented as `Int` or as `Number`/`double`. `5` is a `Int`, while `5.0` is a `Number`.
Since there are scenarios where it is important if it is one and not the other, like array indices for example.

jsonh _will not_ do any conversions for you, so if you `AsNumber()` a `5` value, you will get `nullptr` (but you will get not get a `nullptr` if you `AsNumber()` a `5.0`).


4. Write some important data back

```cpp
auto root = std::make_unique<Object>();
root->object["meaning_of_life"] = std::make_unique<Int>(42);
std::string json_string = Print(root.get(), print_flags::Json, PrettyPrinter::Pretty());
// you may want to use PrettyPrinter::Compact() depending on your needs
```

A more complete (and compileable) example can be found in [example.cc](https://github.com/madeso/json/blob/master/example.cc).

## USP (unique selling points)

-   Line numbers are available after json has loaded successfully

## License

[zlib](https://opensource.org/licenses/Zlib)

## Limitations

Currently the array and object parsing is stack based. What does this mean? Given a json file 1.json containing [1] and another 3.json containing [[[3]]] the library currently parses 1000.json without a problem on my test machines but encounters a stack overflow on 1000000.json. (See issue [#10](https://github.com/madeso/json/issues/10))

The current implementation of the object uses a std::map for storing the key/values but according to the specification there can be many entries with the same key. A conforming json parser can introduce additional limits so this is one. Currently, all but the last key is ignored. (See issue [#11](https://github.com/madeso/json/issues/11))

Currently the root must be either a object or a array. ECMA-404 is a bit unclear if this required.
