cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(jsonh)


# force visual studio and the like to only have debug and release builds
if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Debug Release)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)
endif()


add_library(jsonh_project_options INTERFACE)
add_library(jsonh_project_warnings INTERFACE)

target_compile_features(jsonh_project_options INTERFACE cxx_std_17)

# enable all warnings
# if(MSVC)
#  add_compile_options(/W4)
#else()
#  add_compile_options(-Wall -Wextra -Wpedantic)
#endif()

add_subdirectory(src)


###############################################################################
# simple json cli
set(main_src examples/main.cc)
source_group("" FILES ${main_src})
add_executable(json ${main_src})
target_link_libraries(json
    PUBLIC jsonh
    PRIVATE jsonh_project_options jsonh_project_warnings
)



###############################################################################
# example
set(example_src examples/example.cc)
source_group("" FILES ${example_src})
add_executable(example ${example_src})
target_link_libraries(example
    PUBLIC jsonh
    PRIVATE jsonh_project_options jsonh_project_warnings
)


###############################################################################
# unit tests
set(tests_src
    tests/main.cc
    tests/checker.cc
    tests/coverage.cc
    tests/jsonh.cc
    tests/spec.cc
    tests/utf8.cc
    tests/dup.cc
    tests/print.cc
    tests/roundtrip.cc
    tests/osx.cc
)
source_group("" FILES ${tests_src})
add_executable(tests ${tests_src})
target_include_directories(tests PUBLIC SYSTEM external/catch/)
target_link_libraries(tests
    PUBLIC jsonh
    PRIVATE jsonh_project_options jsonh_project_warnings
)
