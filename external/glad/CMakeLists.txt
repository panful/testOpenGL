
project ("GLAD_LIB_GENERATOR" LANGUAGES C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${3rdPartyArchiveOutputDir})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/glad DESTINATION ${3rdPartyHeadersOutputDir}/glad)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/KHR DESTINATION ${3rdPartyHeadersOutputDir}/glad)

set(target_name "glad")
add_library(${target_name} STATIC "include/glad/glad.h" "include/KHR/khrplatform.h" "src/glad.c")
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
