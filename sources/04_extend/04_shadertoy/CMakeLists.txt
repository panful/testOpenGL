set(target_name "04_04_shadertoy")

add_executable (${target_name} "main.cpp")

# glad glfw
target_include_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/includes/glad 
    ${PROJECT_SOURCE_DIR}/includes/glfw
    ${PROJECT_SOURCE_DIR}/includes/glm
    ${PROJECT_SOURCE_DIR}/includes/stb_image
)
# common.hpp
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/base)

target_link_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/libs)
target_link_libraries(${target_name} PRIVATE glad glfw)

add_custom_command(TARGET ${target_name}
    PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${target_name}>/shaders
)
