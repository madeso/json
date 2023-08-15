###############################################################################
# simple json cli
set(main_src main.cc)
source_group("" FILES ${main_src})
add_executable(json ${main_src})
target_link_libraries(json
    PUBLIC
        jsonh::jsonh
    PRIVATE
        jsonh::project_options
        jsonh::project_warnings
)



###############################################################################
# example
set(example_src example.cc)
source_group("" FILES ${example_src})
add_executable(example ${example_src})
target_link_libraries(example
    PUBLIC
        jsonh::jsonh
    PRIVATE
        jsonh::project_options
        jsonh::project_warnings
)
