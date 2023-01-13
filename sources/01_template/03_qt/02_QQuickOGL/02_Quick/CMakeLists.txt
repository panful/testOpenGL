cmake_minimum_required (VERSION 3.8)

set(CMAKE_AUTOMOC ON) # Meta-Object Compiler
set(CMAKE_AUTORCC OFF) # Resource Compiler
set(CMAKE_AUTOUIC ON) # User Interface Compiler

set(target_name "04_02_02_QQuickOGL")

set(SRCS 
    source/ViewerItem.cpp
    source/FboRender.cpp
)

set(HEADERS
    include/ViewerItem.h
    include/FboRender.h
)

set(RSCS
    resource/04_02_02.qml
)

add_executable (${target_name} main.cpp ${SRCS} ${HEADERS} ${RSCS})

find_package(Qt6 COMPONENTS Core Gui Quick Qml  REQUIRED)

target_link_libraries(${target_name} PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS ${target_name} RUNTIME DESTINATION .)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/04_02_02.qml DESTINATION ./)