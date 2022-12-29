###############################################################################
# simple json cli
set(main_src main.cc)
source_group("" FILES ${main_src})
add_executable(json ${main_src})
target_link_libraries(json
    PUBLIC jsonh
    PRIVATE jsonh_project_options jsonh_project_warnings
)



###############################################################################
# example
set(example_src example.cc)
source_group("" FILES ${example_src})
add_executable(example ${example_src})
target_link_libraries(example
    PUBLIC jsonh
    PRIVATE jsonh_project_options jsonh_project_warnings
)
