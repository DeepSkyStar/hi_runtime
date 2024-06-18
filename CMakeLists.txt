cmake_minimum_required(VERSION 3.15)

if(DEFINED ESP_PLATFORM)
else()
    project(hi_runtime)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message("BEGIN hi_runtime build.")

file(GLOB_RECURSE SRC_FILES
"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# ESP_PLATFORM
# Ref https://cmake.org/cmake/help/latest/variable/CMAKE_SYSTEM_NAME.html#variable:CMAKE_SYSTEM_NAME

# build for different sys.
if(DEFINED ESP_PLATFORM)

    idf_component_register(SRCS
                    ${SRC_FILES}
                    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
                    )
    add_definitions(-D_HI_FREERTOS=1)
    add_definitions(-D_HI_ESP32=1)

elseif("${STM_PLATFORM}" STREQUAL "stm32")
    project(hi_runtime)

    find_package(sdk)
    include(${sdk_LIB_DIRS_DEBUG}/tral_if/buildinfo.cmake)

    add_definitions(-D_HI_FREERTOS=1)
    add_definitions(-D_HI_STM32=1)
    
    add_library(hi_runtime)
    
    target_link_libraries(hi_runtime
            sdk::sdk
    )
    
    target_sources(hi_runtime PRIVATE ${SRC_FILES})

    target_include_directories(hi_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    file(GLOB_RECURSE HI_RUNTIME_HEADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HI_RUNTIME_HEADERS}")

    install(TARGETS ${PROJECT_NAME})
else()
    add_compile_definitions(PUBLIC
    $<$<PLATFORM_ID:Windows>:_HI_WIN=1>
    $<$<PLATFORM_ID:Linux>:_HI_LINUX=1>
    $<$<PLATFORM_ID:iOS>:_HI_IOS=1>
    $<$<PLATFORM_ID:Android>:_HI_ANDROID=1>
    $<$<PLATFORM_ID:Darwin>:_HI_MACOS=1>
    $<$<PLATFORM_ID:tvOS>:_HI_TVOS=1>
    $<$<PLATFORM_ID:watchOS>:_HI_WATCHOS=1>
    $<$<PLATFORM_ID:freertos>:_HI_FREERTOR=1>
    ) 
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
    add_compile_options(-g) # 添加调试信息
    add_compile_options(-O0) # 禁用优化
endif()
    # if (NOT BUILD_ONCE_HI_RUNTIME)

        add_library(hi_runtime)

        target_sources(hi_runtime PRIVATE ${SRC_FILES})

        target_include_directories(hi_runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

        find_package(Threads REQUIRED)

        target_link_libraries(hi_runtime ${CMAKE_THREAD_LIBS_INIT} pthread)
        # set(BUILD_ONCE_HI_RUNTIME ON CACHE BOOL "hi_runtime has been built")
    # endif()

endif()


message("END hi_runtime build.")
