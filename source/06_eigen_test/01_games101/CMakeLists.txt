cmake_minimum_required(VERSION 3.8)

set(target_name "06_01_games101")

add_executable(${target_name} main.cpp)

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../00_3rdparty/eigen3)

install(TARGETS ${target_name} RUNTIME DESTINATION .)