set(CMAKE_AUTOMOC ON)  # Meta-Object Compiler
set(CMAKE_AUTORCC OFF) # Resource Compiler
set(CMAKE_AUTOUIC OFF) # User Interface Compiler

set(target_name "01_03_01_01_QOpenGLWidget")

find_package(Qt6 COMPONENTS REQUIRED Core Widgets OpenGLWidgets)

add_executable (${target_name} "main.cpp" "GLWidget.h" "GLWidget.cpp")

target_link_libraries(${target_name} PRIVATE Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets)
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
