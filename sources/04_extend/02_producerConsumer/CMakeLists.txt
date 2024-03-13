set(target_name "04_02_producerConsumer")

add_executable (${target_name} "main.cpp")

target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/includes/glad ${PROJECT_SOURCE_DIR}/includes/glfw)
target_link_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/libs) 
target_link_libraries(${target_name} PRIVATE glad glfw)
