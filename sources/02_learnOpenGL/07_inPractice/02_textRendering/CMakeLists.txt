set(target_name "02_07_02_textRendering")

add_executable (${target_name} "main.cpp")

# freetype头文件
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/includes/freetype2)
# glad glfw glm freetype头文件
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/includes)
# common.hpp
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/includes)
# glad glfw 库目录
target_link_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/${CMAKE_SYSTEM_NAME}) 
# glad glfw 库文件
target_link_libraries(${target_name} PRIVATE glad glfw3 freetype)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION .)

add_custom_command(TARGET ${target_name}
    PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${target_name}>/resources
)
