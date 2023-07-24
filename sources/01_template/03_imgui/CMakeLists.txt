set(target_name "01_03_imgui")

set(IMGUI_SRC
    ${PROJECT_SOURCE_DIR}/3rdparty/includes/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/3rdparty/includes/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/3rdparty/includes/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/3rdparty/includes/imgui/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/3rdparty/includes/imgui/imgui_impl_opengl3.cpp
    ${PROJECT_SOURCE_DIR}/3rdparty/includes/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/3rdparty/includes/imgui/imgui_widgets.cpp
)

add_executable (${target_name} "main.cpp" ${IMGUI_SRC})

# glad glfw imgui 头文件
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/3rdparty/includes)
# glad glfw 库目录
target_link_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/${CMAKE_SYSTEM_NAME}) 
# glad glfw 库文件
target_link_libraries(${target_name} PRIVATE glad glfw3)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
