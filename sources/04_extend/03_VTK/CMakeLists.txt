set(target_name "04_03_VTK")

set(HEADERS
    Core/include/actor.h
    Core/include/camera.h
    Core/include/geometry.h
    Core/include/interactor.h
    Core/include/interactorStyle.h
    Core/include/mapper.h
    Core/include/object.h
    Core/include/renderer.h
    Core/include/new.h
    Core/include/smartpointer.h
    Core/include/smartpointerbase.h
    Core/include/window.h
    Core/include/objectbase.h
    Core/include/meta.h
    Core/include/objectFactory.h
    Core/include/points.h
    Core/include/cells.h
    Core/include/property.h
    Core/include/dataArray.h
    Core/include/lookupTable.h
    Core/include/texture.h
    Core/include/interactorStyleTrackballCamera.h
    OpenGL/include/OpenGLObjectFactory.h
    OpenGL/include/OpenGLActor.h
    OpenGL/include/OpenGLMapper.h
    OpenGL/include/OpenGLRenderer.h
    OpenGL/include/OpenGLBufferObject.h
    OpenGL/include/OpenGLVertexBufferObject.h
    OpenGL/include/OpenGLIndexBufferObject.h
    OpenGL/include/OpenGLVertexArrayObject.h
    OpenGL/include/OpenGLShaderProgram.h
    OpenGL/include/OpenGLProperty.h
    OpenGL/include/OpenGLVertexBufferObjectGroup.h
    OpenGL/include/OpenGLTextureBufferObject.h
    OpenGL/include/OpenGLTexture.h
)
set(SRCS
    Core/src/actor.cpp
    Core/src/camera.cpp
    Core/src/geometry.cpp
    Core/src/interactor.cpp
    Core/src/interactorStyle.cpp
    Core/src/mapper.cpp
    Core/src/object.cpp
    Core/src/renderer.cpp
    Core/src/smartpointerbase.cpp
    Core/src/window.cpp
    Core/src/objectbase.cpp
    Core/src/objectFactory.cpp
    Core/src/points.cpp
    Core/src/cells.cpp
    Core/src/property.cpp
    Core/src/dataArray.cpp
    Core/src/lookupTable.cpp
    Core/src/texture.cpp
    Core/src/interactorStyleTrackballCamera.cpp
    OpenGL/src/OpenGLObjectFactory.cpp
    OpenGL/src/OpenGLActor.cpp
    OpenGL/src/OpenGLMapper.cpp
    OpenGL/src/OpenGLRenderer.cpp
    OpenGL/src/OpenGLBufferObject.cpp
    OpenGL/src/OpenGLVertexBufferObject.cpp
    OpenGL/src/OpenGLIndexBufferObject.cpp
    OpenGL/src/OpenGLVertexArrayObject.cpp
    OpenGL/src/OpenGLShaderProgram.cpp
    OpenGL/src/OpenGLProperty.cpp
    OpenGL/src/OpenGLVertexBufferObjectGroup.cpp
    OpenGL/src/OpenGLTextureBufferObject.cpp
    OpenGL/src/OpenGLTexture.cpp
)

add_executable(${target_name} "main.cpp" ${HEADERS} ${SRCS})
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Core/include)
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL/include)
target_include_directories(${target_name} PRIVATE 
    ${PROJECT_SOURCE_DIR}/includes/glad 
    ${PROJECT_SOURCE_DIR}/includes/glfw
    ${PROJECT_SOURCE_DIR}/includes/glm
)
target_link_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/libs) 
target_link_libraries(${target_name} PRIVATE glad glfw)
