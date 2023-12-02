set(target_name "01_04_glm")

add_executable (${target_name} "main.cpp" "print_glm.h")

target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/includes/glm)
