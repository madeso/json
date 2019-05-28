# JSON parser for humans

[![Build Status](https://www.travis-ci.org/madeso/json.svg?branch=master)](https://www.travis-ci.org/madeso/json)
[![Coverage Status](https://coveralls.io/repos/github/madeso/json/badge.svg?branch=master)](https://coveralls.io/github/madeso/json?branch=master)

JSON parser for humans, or jsonh for short, is designed to be a simple cross-platform input/output "stb style" library to read [JSON](https://json.org/) and JSON-like files written by humans into a DOM like structure.

**What does simple input/output library mean?** It means there are no extra functions to create json making the library more complex. There is also no custom struct to json functions. If this is not what you are looking for, perhaps have a look at [JSON for Modern C++](https://github.com/nlohmann/json) instead.

**What does "written by humans" mean?** It means that given a choice between helpful error messages/syntax and performance, helpfulness will always win. This means that for every json value, there is a corresponding line/column entry so that when you search in your database for the user entered value and don't find it you can let the user know where in the json the value was requested. If this is not what you are looking for, perhaps have a look at [rapidjson](https://github.com/Tencent/rapidjson) instead.

**What does stb style library mean?** It means that it is distributed as a single header without templates and external dependencies and in one file you have to define a macro to "implement the implementations". See the [stb repo](https://github.com/nothings/stb) and [stb howto](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt) for a more detailed information.

**What does cross platform mean?** It means that currently each commit it built and tested on with both gcc and clang on both linux and osx. It is also tested on windows with visual studio 2017. It may work on others, but those are not tested (ports welcome!).

## How to use

1. Add jsonh.h to your project in some way.

2. In _one_ file, #define JSONH\_IMPLEMENTATION to get the implementation for the classes and functions.

```cpp
// in main.cpp
#define JSONH_IMPLMENENTATION
#include "jsonh.h"

// in other_files.cpp
#include "json.h"
```

3. Use it!

```cpp
// 3a. Parse string into DOM structure
auto j = Parse(json_string, ParseFlags::Json);

// 3b. Check for errors
if(!j) { std::cerr << j; return 0; }
// or
if(j.HasErrors()) { std::cerr << j; return 0; };
// or
if(!j.errors.empty()) { std::cerr << j; return 0; };
// or
if(j.value == nullptr) { std::cerr << j; return 0; };

// 3c. Use j.value
std::shared_ptr<Value> root = j.value;

// the root of a json can eiter be a array or a object.
Array* arr = root->AsArray();
Object* obj = root->AsObject();

// Both functions return a nullptr if the root isn't array or a object. 

// Array::array is a std::vector of shared_ptr to Value
// 
// struct Array : public Value
// {
//   std::vector<std::shared_ptr<Value>> array;
// };


// Object::object is a std::map of std::string to shared_ptr to Value
// 
// struct Object : public Value
// {
//   std::map<std::string, std::shared_ptr<Value>> object;
// };
 
```

Other types of values are

* Null (AsNull)
* Bool(AsBool, Bool::boolean)
* Int(AsInt(), Int::integer)
* Number(AsNumber(), Number::number)
* String (AsString(), String::string)

Numbers in jsonh are either represented as Int or as Number. 0 is a Int, while 0.0 is a Number.
Since there are scenarios where it is important if it is one and not the other,
array indices for example.

jsonh _will not_ do any conversions for you. If you AsNumber a 5 value, you will get nullptr (but you will get a non-null if you AsNumber a 5.0).

```cpp
// 4. write some important data back
auto root = std::make_shared<Object>();
root->object["meaning_of_life"] = std::make_shared<Int>(42);
std::string json_string = Print(root.get(), PrintFlags::Json, PrettyPrinter::Pretty()); 
// you may want to use PrettyPrinter::Compact() depending on your needs
```

A more complete (and compileable) example can be found in [example.cc](https://github.com/madeso/json/blob/master/example.cc).

## Roadmap

Some will get done, others might.

* Make faster and less dependent on STL
* Make usable in non-modern C++ and C?
* macro arguments to add prefixes or custom names for structs
* [Relaxed json](https://github.com/Tencent/rapidjson/issues/36)
* [s json](https://github.com/Autodesk/sjson) from [autodesk stingray](http://help.autodesk.com/view/Stingray/ENU/?guid=__stingray_help_managing_content_sjson_html)
* [json 5](https://json5.org)
* [h json](https://hjson.org/)

## USP (unique selling points)

* Line numbers are available after json has loaded successfully
* Single header file

## License

MIT

## Limitations

Currently the array and object parsing is stack based. What does this mean? Given a json file 1.json containing [1] and another 3.json containing [[[3]]] the library currently parses 1000.json without a problem on my test machines but encounters a stack overflow on 1000000.json.

The current implementation of the object uses a std::map for storing the key/values but according to the specification there can be many entries with the same key. A conforming json parser can introduce additional limits so this is one. Currently, all but the last key is ignored.
