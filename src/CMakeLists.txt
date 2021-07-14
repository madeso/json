set(src
    jsonh/jsonh.cc
    jsonh/jsonh.h
)
add_library(jsonh STATIC ${src})
target_link_libraries(jsonh
    PRIVATE jsonh_project_options jsonh_project_warnings
)
target_include_directories(jsonh
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
