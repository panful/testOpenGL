set(target_name "01_06_ktx")

find_package(KTX REQUIRED)

add_executable (${target_name} "main.cpp")

target_link_libraries(${target_name} PRIVATE KTX::ktx)

# 获取ktx的动态库
get_target_property(ktx_dll KTX::ktx IMPORTED_LOCATION_RELEASE)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION .)
install(FILES ${ktx_dll} DESTINATION .)

add_custom_command(TARGET ${target_name}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy ${ktx_dll} $<TARGET_FILE_DIR:${target_name}>
)
