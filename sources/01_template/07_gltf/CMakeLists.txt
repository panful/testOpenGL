set(target_name "01_07_gltf")

add_executable(${target_name} "main.cpp")

target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/includes/tinygltf)
target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/includes/glm)
