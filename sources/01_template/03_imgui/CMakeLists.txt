set(target_name "01_03_imgui")

add_executable(${target_name} "main.cpp")

target_include_directories(${target_name} PRIVATE 
    ${3rdPartyHeadersOutputDir}/imgui
    ${3rdPartyHeadersOutputDir}/glad
    ${3rdPartyHeadersOutputDir}/glfw
)

target_link_directories(${target_name} PRIVATE ${3rdPartyArchiveOutputDir})
target_link_libraries(${target_name} PRIVATE glad glfw imgui)
