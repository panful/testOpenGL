
set(root_path ${PROJECT_SOURCE_DIR}/sources/02_learnOpenGL)

macro(CopyShaders shader_path)
    if(EXISTS "${shader_path}/shaders")
        add_custom_command(TARGET ${target_name}
            POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E
            copy_directory ${shader_path}/shaders $<TARGET_FILE_DIR:${target_name}>/shaders
        )
    endif()
endmacro()

macro(TargetLinkInclude)
    target_include_directories(${target_name} PRIVATE
        ${PROJECT_SOURCE_DIR}/base
        ${3rdPartyHeadersOutputDir}/glad
        ${3rdPartyHeadersOutputDir}/glfw
        ${3rdPartyHeadersOutputDir}/glm
        ${3rdPartyHeadersOutputDir}/imgui
        ${3rdPartyHeadersOutputDir}/ktx
        ${3rdPartyHeadersOutputDir}/stb_image
        ${3rdPartyHeadersOutputDir}/tinygltf
        ${3rdPartyHeadersOutputDir}/freetype
    )

    target_link_directories(${target_name} PRIVATE ${3rdPartyArchiveOutputDir})
    target_link_libraries(${target_name} PRIVATE glad glfw imgui ktx freetype)
endmacro()

function(BuildTarget)
    file(GLOB subdirectories ${root_path}/*/*)
    foreach(subdir ${subdirectories})
        string(REGEX MATCH ".*/([0-9]+)_([^/]+)/([0-9]+)_([^/]+)/([0-9]+)_(.*)$" match_result ${subdir})
        if(match_result)
            file(GLOB target_sources ${subdir}/*.cpp)
            if(target_sources)
                set(target_name "${CMAKE_MATCH_1}_${CMAKE_MATCH_3}_${CMAKE_MATCH_5}_${CMAKE_MATCH_6}")
                add_executable(${target_name} ${target_sources})
                TargetLinkInclude()
                CopyShaders(${subdir})
            else()
                file(GLOB subdirectories2 ${subdir}/*)
                foreach(subdir2 ${subdirectories2})
                    string(REGEX MATCH ".*/([0-9]+)_([^/]+)/([0-9]+)_([^/]+)/([0-9]+)_([^/]+)/([0-9]+)_(.*)$" match_result ${subdir2})
                    file(GLOB target_sources ${subdir2}/*.cpp)
                    if(match_result)
                        set(target_name "${CMAKE_MATCH_1}_${CMAKE_MATCH_3}_${CMAKE_MATCH_5}_${CMAKE_MATCH_7}_${CMAKE_MATCH_8}")
                        add_executable(${target_name} ${target_sources})
                        TargetLinkInclude()
                        CopyShaders(${subdir2})
                    endif()
                endforeach()
            endif()
        endif()
    endforeach()
endfunction(BuildTarget)
