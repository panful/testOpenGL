set(target_name "02_03_modelLoading")

add_executable(${target_name} "main.cpp")

# common.hpp
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/includes)
# glad glfw assimp 头文件
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/includes)
# gltf
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty2/glTF)
# glad glfw assimp 库目录
target_link_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/${CMAKE_SYSTEM_NAME}) 
# glad glfw assimp 库文件
target_link_libraries(${target_name} PRIVATE glad glfw3 assimp)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION .)
install(FILES ${PROJECT_SOURCE_DIR}3rdparty/bin/assimp-vc143-mt.dll DESTINATION .)

add_custom_command(TARGET ${target_name}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${target_name}>/resources
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_if_different ${PROJECT_SOURCE_DIR}/3rdparty/bin/assimp-vc143-mt.dll $<TARGET_FILE_DIR:${target_name}>/assimp-vc143-mt.dll
)
