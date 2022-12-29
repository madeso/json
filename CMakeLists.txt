cmake_minimum_required(VERSION 3.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(jsonh)

###############################################################################
# common properties
add_library(jsonh_project_options INTERFACE)
add_library(jsonh::project_options ALIAS jsonh_project_options)
target_compile_features(jsonh_project_options INTERFACE cxx_std_17)

add_library(jsonh_project_warnings INTERFACE)
add_library(jsonh::project_warnings ALIAS jsonh_project_warnings)
# todo(Gustav): add extra compiler warnings




###############################################################################
# main lib
add_subdirectory(src)




###############################################################################
# examples
add_subdirectory(examples)




###############################################################################
# tests
add_library(jsonh_catch INTERFACE)
target_include_directories(jsonh_catch
    INTERFACE SYSTEM external/catch
)
add_library(jsonh::catch ALIAS jsonh_catch)

include(CTest)
add_subdirectory(tests)
