set(src
    jsonh/jsonh.cc
    jsonh/jsonh.h
)
add_library(jsonh STATIC ${src})
target_link_libraries(jsonh
    PRIVATE
        jsonh::project_options
        jsonh::project_warnings
)
target_include_directories(jsonh
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
add_library(jsonh::jsonh ALIAS jsonh)
