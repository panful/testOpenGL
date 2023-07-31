set(target_name "02_01_03_texture")

find_package(KTX REQUIRED)

add_executable (${target_name} "main.cpp")

# common.hpp
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/includes)
# glad glfw 头文件
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/includes)
# glad glfw 库目录
target_link_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/${CMAKE_SYSTEM_NAME}) 
# glad glfw ktx 库文件
target_link_libraries(${target_name} PRIVATE glad glfw3 KTX::ktx)

# 获取ktx的动态库
get_target_property(ktx_dll KTX::ktx IMPORTED_LOCATION_RELEASE)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION .)
install(FILES ${ktx_dll} DESTINATION .)

add_custom_command(TARGET ${target_name}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${target_name}>/resources
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy ${ktx_dll} $<TARGET_FILE_DIR:${target_name}>
)
