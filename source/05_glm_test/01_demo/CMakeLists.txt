cmake_minimum_required (VERSION 3.8)

set(target_name "05_01_demo")

add_executable (${target_name} "main.cpp")

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/includes)

install(TARGETS ${target_name} RUNTIME DESTINATION ./)
