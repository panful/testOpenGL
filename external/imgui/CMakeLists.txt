
project ("IMGUI_LIB_GENERATOR")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${3rdPartyArchiveOutputDir})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imconfig.h DESTINATION ${3rdPartyHeadersOutputDir}/imgui/imgui)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgui.h DESTINATION ${3rdPartyHeadersOutputDir}/imgui/imgui)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_glfw.h DESTINATION ${3rdPartyHeadersOutputDir}/imgui/imgui)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_opengl3.h DESTINATION ${3rdPartyHeadersOutputDir}/imgui/imgui)

set(target_name "imgui")
file(GLOB IMGUI_SOURCES "*.cpp")
add_library(${target_name} STATIC ${IMGUI_SOURCES})
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../glfw/include)
