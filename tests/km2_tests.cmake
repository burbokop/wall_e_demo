cmake_minimum_required(VERSION 3.18.4)


add_executable(km2_tests
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/jit_spec.h
    ${CMAKE_CURRENT_LIST_DIR}/src/jit_spec.cpp
    )

get_filename_component(PARENT_LIST_DIR ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
target_include_directories(km2_tests PRIVATE ${PARENT_LIST_DIR})
target_link_libraries(km2_tests
    wall_e
    km2
    )


enable_testing()
add_test(km2_tests km2_tests)

#add_custom_command(
#     TARGET km2_tests
#     COMMENT "km2_tests"
#     POST_BUILD
#     COMMAND km2_tests
#)
