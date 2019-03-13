cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(jsoni)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# compiler flags
if(${MSVC})
  add_definitions(/D _CRT_SECURE_NO_WARNINGS)

  # character set to Not Set=ASCII/SBCS
  # http://public.kitware.com/Bug/view.php?id=12189
  add_definitions(/D _SBCS)
endif()

# enable all warnings
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)

  # fix the code for this
  add_compile_options(-Wno-unused-parameter)
  add_compile_options(-Wno-ignored-qualifiers)
endif()

# force visual studio and the like to only have debug and release builds
if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Debug Release)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}"
    CACHE STRING "Reset the configurations to what we need" FORCE)
endif()


set(tests_src
  tests_main.cc
  tests_checker.cc
  tests_jsonh.cc
  tests_spec.cc
  tests_utf8.cc
  tests_whitebox.cc
  )
source_group("" FILES ${tests_src})
include_directories(SYSTEM external/catch/)
add_executable(tests ${tests_src})
enable_testing()
add_test(tests tests)

