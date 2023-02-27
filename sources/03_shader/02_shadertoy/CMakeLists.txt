cmake_minimum_required (VERSION 3.8)

set(target_name "03_02_shadertoy")

add_executable (${target_name} "main.cpp")

# common.hpp
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/includes)
# glad glfw
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/includes)
target_link_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/${CMAKE_SYSTEM_NAME})
target_link_libraries(${target_name} PRIVATE glad glfw3)

add_custom_command(TARGET ${target_name}
    PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${target_name}>/resources
)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION .)
