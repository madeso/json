# JSON parser for humans

JSON parser for humans, or jsonh for short, is designed to be a simple input/output "stb style" library to read [JSON](https://json.org/) and JSON-like files written by humans into a DOM like structure.

**What does simple input/output library mean?** It means there are no extra functions to create json making the library more complex. There is also no custom struct to json functions. If this is not what you are looking for, perhaps have a look at [JSON for Modern C++](https://github.com/nlohmann/json) instead.

**What does "written by humans" mean?** It means that given a choice between helpful error messages/syntax and performance, helpfullness will always win. This means that for every json value, there is a corresponing line/column entry so that when you search in your database for the user entered value and don't find it you can let the user know where in the json the value was requested. If this is not what you are looking for, perhaps have a look at [rapidjson](https://github.com/Tencent/rapidjson) instead.

**What does stb style library mean?** It means that it is distributet as a single header without templates and external dependencies and in one file you have to define a macro to "implement the implementations". See the [stb repo](https://github.com/nothings/stb) and [stb howto](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt) for a more detailed infomation.

## Roadmap

1. Make sure it parses regular JSON files correctly.
2. Add a simple output/to string function to serialize dom back to disk/string.
3. Make faster and less dependent on STL

## How to use

Add jsonh.h to your project in some way. In one file, #define JSONH\_IMPLEMENTATION to get the implementation for the classes and functions.

```cpp
// 1. Parse string into DOM structure
auto j = Parse(json_string);

// 2. Check for errors
if(!j) { std::cerr << j; return 0; }
// or
if(j.HasErrors()) { std::cerr << j; return 0; };
// or
if(!j.errors.empty()) { std::cerr << j; return 0; };
// or
if(j.value == nullptr) { std::cerr << j; return 0; };

// 3. Use j.value
```

## Possible future roadmap

* Make usable in not modern C++ and C?
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

Currently the array and object parsing is stack based. What does this mean? Given a json file 1.json containing [1] and another 3.json containing [[[3]]] the library currently parses 1000.json without a problem but encounter a stack overflow on 1000000.json. So it shouldn't be any limitations for manually written json but still is something worth mentioning.

The current implementation of the object uses a std::map for storing the key/values but according to the specification there can be many entries with the same key. A conforming json parser can introduce additional limits so this is one.

