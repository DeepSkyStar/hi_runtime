cmake_minimum_required(VERSION 3.15)
project(hi_runtime_test)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../build")

add_executable(hi_runtime_test main.c)
target_link_libraries(hi_runtime_test PUBLIC hi_runtime)
