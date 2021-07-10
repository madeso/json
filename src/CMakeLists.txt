set(src
  jsonh/jsonh.h
  jsonh/internal.parse_functions.h jsonh/internal.parse_functions.cc
  jsonh/internal.parser.h jsonh/internal.parser.cc

  jsonh/error.cc
  jsonh/location.cc
  jsonh/parse.cc
  jsonh/parse.result.cc
  jsonh/print.cc
  jsonh/visitor.cc
  jsonh/value.cc
  jsonh/value.array.cc
  jsonh/value.bool.cc
  jsonh/value.int.cc
  jsonh/value.null.cc
  jsonh/value.number.cc
  jsonh/value.object.cc
  jsonh/value.string.cc
)
add_library(jsonh STATIC ${src})
target_link_libraries(jsonh
    PRIVATE jsonh_project_options jsonh_project_warnings
)
target_include_directories(jsonh
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
