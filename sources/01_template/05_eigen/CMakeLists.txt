cmake_minimum_required(VERSION 3.8)

set(target_name "01_05_eigen")

add_executable(${target_name} main.cpp)

target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/includes)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
