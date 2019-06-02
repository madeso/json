cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(jsonh)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable all warnings
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# force visual studio and the like to only have debug and release builds
if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Debug Release)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}"
    CACHE STRING "Reset the configurations to what we need" FORCE)
endif()



# simple json cli
set(main_src
  jsonh.h
  main.cc
  )
source_group("" FILES ${main_src})
add_executable(json ${main_src})



# example
set(example_src
  jsonh.h
  example.cc
  )
source_group("" FILES ${example_src})
add_executable(example ${example_src})



# unit tests
set(tests_src
  jsonh.h
  tests_main.cc
  tests_impl.cc
  tests_checker.cc
  tests_coverage.cc
  tests_jsonh.cc
  tests_spec.cc
  tests_utf8.cc
  tests_dup.cc
  tests_print.cc
  tests_roundtrip.cc
  tests_osx.cc
  )
source_group("" FILES ${tests_src})
include_directories(SYSTEM external/catch/)
add_executable(tests ${tests_src})
