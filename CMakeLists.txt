cmake_minimum_required(VERSION 3.15)

# List option for build
# option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# import sources
file(GLOB_RECURSE SRC_FILES
"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

message("list ${SRC_FILES}")

# build for different sys.
if(COMMAND idf_component_register)
    idf_component_register(
                    SRC_FILES
                    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
    message("idf_component_register defined.")
else()
    message("idf_component_register undefined.")
    project(hi_runtime)
    add_library(hi_runtime STATIC ${SRC_FILES})
    target_include_directories(hi_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")
endif()



