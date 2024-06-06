cmake_minimum_required(VERSION 3.15)
project(main)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ hi_runtime)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG)
  add_compile_options(-g) # 添加调试信息
  add_compile_options(-O0) # 禁用优化
endif()

add_executable(main main.c)

target_link_libraries(main PUBLIC hi_runtime)

