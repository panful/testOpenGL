cmake_minimum_required (VERSION 3.12)
project ("OpenGLProject")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/target)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/target)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/target)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /utf-8 /EHsc")
endif()

set(3rdPartyArchiveOutputDir "${CMAKE_CURRENT_SOURCE_DIR}/libs")
set(3rdPartyHeadersOutputDir "${CMAKE_CURRENT_SOURCE_DIR}/includes")

add_subdirectory("external")
add_subdirectory("sources")
