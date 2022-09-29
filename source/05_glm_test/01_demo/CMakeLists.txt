
cmake_minimum_required (VERSION 3.8)

set(target_name "05_01_demo")

find_package(GLM
    QUIET
    PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../00_3rdparty/glm/lib/cmake/glm
    NO_DEFAULT_PATH
)

add_executable (${target_name} "main.cpp")

target_link_libraries(${target_name} PRIVATE glm::glm)

install(TARGETS ${target_name}
        RUNTIME DESTINATION ./
)