cmake_minimum_required(VERSION 3.16)
project(Alchemication)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT TARGET Kos_Engine)
    message(FATAL_ERROR "Kos_Engine target not found. Make sure Engine/ is built first.")
endif()


file(GLOB_RECURSE EDITOR_SOURCES
    Application/*.cpp
    Configs/*.cpp
)

add_executable(Alchemication ${EDITOR_SOURCES})
if (MSVC)
    # Build without console window
    target_link_options(Alchemication PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
endif()
add_dependencies(Alchemication SCRIPTS)

target_link_libraries(Alchemication PRIVATE Kos_Engine)

target_include_directories(Alchemication
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MSVC)
    target_compile_options(Alchemication PRIVATE /Zc:preprocessor )
endif()

