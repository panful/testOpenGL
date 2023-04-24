set(target_name "01_02_glew_glfw")

# 如果不是Windows平台不生成该项目
if(NOT WIN32)
    return()
endif()

add_executable(${target_name} "main.cpp")

# glew glfw 头文件
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/includes)
# glew glfw 库目录
target_link_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/3rdparty/libs/Debug/${CMAKE_SYSTEM_NAME})

# 报错：无法解析的外部符号 __imp_glViewport
# 添加链接库 opengl32
target_link_libraries(${target_name} PRIVATE glfw3 glew32d opengl32)

# 拷贝glew32d.dll到生成目录
add_custom_command(TARGET ${target_name}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${PROJECT_SOURCE_DIR}/3rdparty/bin $<TARGET_FILE_DIR:${target_name}>
)

install(TARGETS ${target_name} RUNTIME DESTINATION .)

# 拷贝glew32d.dll到安装目录
install(FILES  ${PROJECT_SOURCE_DIR}/3rdparty/bin/glew32d.dll DESTINATION .)
