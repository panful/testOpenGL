
project ("KTX_LIB_GENERATOR")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${3rdPartyArchiveOutputDir})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/ktx.h DESTINATION ${3rdPartyHeadersOutputDir}/ktx)

set(target_name "ktx")
set(KTX_SOURCES
    lib/texture.c
    lib/hashlist.c
    lib/checkheader.c
    lib/swap.c
    lib/memstream.c
    lib/filestream.c
)
add_library(${target_name} STATIC ${KTX_SOURCES})
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
