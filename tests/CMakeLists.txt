set(tests_src main.cc
    checker.cc
    coverage.cc
    dup.cc
    jsonh.cc
    osx.cc
    print.cc
    roundtrip.cc
    spec.cc
    utf8.cc
)
source_group("" FILES ${tests_src})
add_executable(tests ${tests_src})
target_link_libraries(tests
    PUBLIC
        jsonh::jsonh
    PRIVATE
        jsonh::project_options
        jsonh::project_warnings
        jsonh::catch
)

include(../external/catch/Catch.cmake)
catch_discover_tests(tests)
