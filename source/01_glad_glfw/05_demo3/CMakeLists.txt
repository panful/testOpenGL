cmake_minimum_required (VERSION 3.8)

set(target_name "01_05_demo3")

add_executable (${target_name} "main.cpp")
# shader.hpp
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../includes)
# glad glfw
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/includes)
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/includes)
# 链接glad glfw
if(WIN32)
    target_link_directories(${target_name} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/Windows) 
elseif(UNIX)
    target_link_directories(${target_name} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/Linux) 
endif()
target_link_libraries(${target_name} PRIVATE glad glfw3)

add_custom_command(TARGET ${target_name}
    PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${target_name}>/resources
)

install(TARGETS ${target_name}
        RUNTIME DESTINATION ./
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION .)
