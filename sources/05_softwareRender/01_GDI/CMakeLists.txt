if(NOT WIN32)
    return()
endif()

set(target_name "05_01_softwareRender_GDI")

add_executable (${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION .)
