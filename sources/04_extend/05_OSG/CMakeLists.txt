set(target_name "04_05_OSG")

set(HEADERS
    Core/include/Object.h
    Core/include/Node.h
    Core/include/Group.h
    Core/include/Visitor.h
    Core/include/Viewer.h
    Core/include/Window.h
    Core/include/CompileTraversal.h
    Core/include/RecordTraversal.h
    Core/include/Camera.h
    Core/include/Geometry.h
    Core/include/GraphicsPipeline.h
    Core/include/State.h
    Core/include/Transform.h
    Core/include/View.h
    Core/include/Animation.h

    Utils/include/Log.h
)
set(SRCS
    Core/src/Object.cpp
    Core/src/Node.cpp
    Core/src/Group.cpp
    Core/src/Visitor.cpp
    Core/src/Viewer.cpp
    Core/src/Window.cpp
    Core/src/CompileTraversal.cpp
    Core/src/RecordTraversal.cpp
    Core/src/Camera.cpp
    Core/src/Geometry.cpp
    Core/src/GraphicsPipeline.cpp
    Core/src/State.cpp
    Core/src/Transform.cpp
    Core/src/View.cpp
    Core/src/Animation.cpp

    Utils/src/Log.cpp
)

add_executable(${target_name} "main.cpp" ${HEADERS} ${SRCS})
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Core/include)
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Utils/include)
target_include_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/includes/glad 
    ${PROJECT_SOURCE_DIR}/includes/glfw
    ${PROJECT_SOURCE_DIR}/includes/glm
)
target_link_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/libs) 
target_link_libraries(${target_name} PRIVATE glad glfw)
