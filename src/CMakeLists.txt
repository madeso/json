set(src
  jsonh/jsonh.h
  jsonh/value.types.h

  jsonh/internal.parse_functions.h jsonh/internal.parse_functions.cc
  jsonh/internal.parser.h jsonh/internal.parser.cc

  jsonh/error.h jsonh/error.cc
  jsonh/location.h jsonh/location.cc
  jsonh/parse.h jsonh/parse.cc
  jsonh/parse.result.h jsonh/parse.result.cc
  jsonh/print.h jsonh/print.cc
  jsonh/visitor.h jsonh/visitor.cc
  jsonh/value.h jsonh/value.cc
  jsonh/value.array.h jsonh/value.array.cc
  jsonh/value.bool.h jsonh/value.bool.cc
  jsonh/value.int.h jsonh/value.int.cc
  jsonh/value.null.h jsonh/value.null.cc
  jsonh/value.number.h jsonh/value.number.cc
  jsonh/value.object.h jsonh/value.object.cc
  jsonh/value.string.h jsonh/value.string.cc
)
add_library(jsonh STATIC ${src})
target_link_libraries(jsonh
    PRIVATE jsonh_project_options jsonh_project_warnings
)
target_include_directories(jsonh
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
