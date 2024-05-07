set(target_name "01_08_glad_win")

add_executable(${target_name} WIN32 "main.cpp")

target_include_directories(${target_name} PRIVATE ${3rdPartyHeadersOutputDir}/glad)

target_link_directories(${target_name} PRIVATE ${3rdPartyArchiveOutputDir})
target_link_libraries(${target_name} PRIVATE glad opengl32)
