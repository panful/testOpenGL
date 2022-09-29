cmake_minimum_required (VERSION 3.8)
project ("OpenGLProject")
set(target_name "OpenGL_Test")

add_executable (${target_name} "main.cpp")

# glad glfw
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/includes)

# 链接glad glfw
if(WIN32)
	target_link_directories(${target_name} PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/Windows) 
elseif(UNIX)
	target_link_directories(${target_name} PRIVATE 
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libs/$<IF:$<CONFIG:Debug>,Debug,Release>/Linux) 
endif()

target_link_libraries(${target_name} PRIVATE glfw3)
target_link_libraries(${target_name} PRIVATE glad)
