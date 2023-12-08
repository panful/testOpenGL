########################################
# 编译 02_learnOpenGL目录下所有项目
#########################################

macro(CopyShaders shader_path)
    if(EXISTS ${shader_path}/shaders)
        add_custom_command(TARGET ${target_name}
            POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E
            copy_directory ${shader_path}/shaders $<TARGET_FILE_DIR:${target_name}>/shaders
        )
    endif()
endmacro()

macro(TargetInclude)
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
endmacro()

set(root_path ${PROJECT_SOURCE_DIR}/sources/02_learnOpenGL)

# 第一个参数是当前项目的路径，其余参数是除glad glfw以外的库名称
function(BuildLearnOpenGL path)
    set(_path_ ${path})
    set(_libs_ ${ARGN})

    string(REGEX MATCH "^([0-9]+)_([^/]+)/([0-9]+)_([^/]+)$" match_result ${_path_})
    if(match_result)
        file(GLOB target_sources ${_path_}/*.cpp)
        if(target_sources)
            set(target_name 02_${CMAKE_MATCH_1}_${CMAKE_MATCH_3}_${CMAKE_MATCH_4})
            add_executable(${target_name} ${target_sources})
            TargetInclude()
            target_link_directories(${target_name} PRIVATE ${3rdPartyArchiveOutputDir})
            target_link_libraries(${target_name} PRIVATE glad glfw ${_libs_})
            CopyShaders(${root_path}/${_path_})
        endif(target_sources)
    else()
        string(REGEX MATCH "^([0-9]+)_([^/]+)/([0-9]+)_([^/]+)/([0-9]+)_([^/]+)$" match_result ${_path_})
        if(match_result)
            file(GLOB target_sources ${_path_}/*.cpp)
            if(target_sources)
                set(target_name 02_${CMAKE_MATCH_1}_${CMAKE_MATCH_3}_${CMAKE_MATCH_5}_${CMAKE_MATCH_6})
                add_executable(${target_name} ${target_sources})
                TargetInclude()
                target_link_directories(${target_name} PRIVATE ${3rdPartyArchiveOutputDir})
                target_link_libraries(${target_name} PRIVATE glad glfw ${_libs_})
                CopyShaders(${root_path}/${_path_})
            endif(target_sources)
        endif(match_result)
    endif(match_result)
endfunction(BuildLearnOpenGL path)
