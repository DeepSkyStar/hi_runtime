cmake_minimum_required(VERSION 3.15)
project(main)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ hi_runtime)

add_executable(main main.c)

target_link_libraries(main PUBLIC hi_runtime)

