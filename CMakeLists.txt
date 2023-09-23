cmake_minimum_required(VERSION 3.5.0)

if(NOT DEFINED JSON_MASTER_PROJECT)
    set(JSON_MASTER_PROJECT OFF)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(JSON_MASTER_PROJECT ON)
    endif()
endif()

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
if(JSON_MASTER_PROJECT)
    add_subdirectory(examples)
endif()




###############################################################################
# tests
if(JSON_MASTER_PROJECT)
    add_library(jsonh_catch INTERFACE)
    target_include_directories(jsonh_catch
        INTERFACE SYSTEM external/catch
    )
    add_library(jsonh::catch ALIAS jsonh_catch)

    include(CTest)
    add_subdirectory(tests)
endif()