set(target_name "02_00_other")

add_executable (${target_name} "main.cpp")

# glad glfw 头文件
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/includes)
# glad glfw 库目录
target_link_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/${CMAKE_SYSTEM_NAME}) 
# glad glfw 库文件
target_link_libraries(${target_name} PRIVATE glad glfw3)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
