set(target_name "02_04_12_scissor")

add_executable (${target_name} "main.cpp")

# glad glfw glm头文件
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/includes)
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/includes)
# glad glfw 库目录
target_link_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/${CMAKE_SYSTEM_NAME}) 
# glad glfw 库文件
target_link_libraries(${target_name} PRIVATE glad glfw3)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION .)

add_custom_command(TARGET ${target_name}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${target_name}>/resources
)
