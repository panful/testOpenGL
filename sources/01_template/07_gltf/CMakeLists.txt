set(target_name "01_07_gltf")

add_executable(${target_name} "main.cpp")
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty2/glTF)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION .)

add_custom_command(TARGET ${target_name}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${target_name}>/resources
)