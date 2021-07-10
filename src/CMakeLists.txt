set(src
  jsonh/jsonh.h
  jsonh/internal.parse_functions.h jsonh/internal.parse_functions.cc
  jsonh/internal.parser.h jsonh/internal.parser.cc

  jsonh/jsonh.cc
)
add_library(jsonh STATIC ${src})
target_link_libraries(jsonh
    PRIVATE jsonh_project_options jsonh_project_warnings
)
target_include_directories(jsonh
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
