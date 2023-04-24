add_subdirectory("01_glad_glfw")
add_subdirectory("02_glew_glfw")

find_package(Qt6)
if(Qt6_FOUND)
    add_subdirectory("03_qt")
else()
    message("------------- Qt Not Found -------------")
endif()

add_subdirectory("04_glm")
add_subdirectory("05_eigen")
