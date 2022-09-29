cmake_minimum_required (VERSION 3.8)

set(target_name "02_01_demo")

find_package(GLFW3
    QUIET
    PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../00_3rdparty/glfw/lib/cmake/glfw3
    NO_DEFAULT_PATH
)

find_package(GLEW
    QUIET
    PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../00_3rdparty/glew/lib/cmake/glew
    NO_DEFAULT_PATH
)

find_package(GLM
    QUIET
    PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../00_3rdparty/glm/lib/cmake/glm
    NO_DEFAULT_PATH
)

if(NOT GLEW_FOUND)
    message(FATAL_ERROR "=== 没有找到glew ===")
endif()

add_executable (${target_name} "main.cpp")

# 链接第三方库glfw,glew
target_link_libraries(${target_name} PRIVATE glfw)
target_link_libraries(${target_name} PRIVATE GLEW::glew)
target_link_libraries(${target_name} PRIVATE glm::glm)

install(TARGETS ${target_name}
        RUNTIME DESTINATION ./
)

# 安装glew32d.dll文件到可执行文件目录
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../00_3rdparty/glew/bin/glew32d.dll DESTINATION .)