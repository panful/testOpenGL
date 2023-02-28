cmake_minimum_required (VERSION 3.12)
project ("OpenGLProject")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/target)

add_subdirectory("sources")
