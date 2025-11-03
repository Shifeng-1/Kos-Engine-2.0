cmake_minimum_required(VERSION 3.16)
project(Kos_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ENGINE_SOURCE
    Config/*.cpp
    Dependencies/glad/*.cpp
    DeSerialization/*.cpp
    Debugging/*.cpp
    ECS/*.cpp
    Graphics/*.cpp
    Inputs/*.cpp
    Pathfinding/*.cpp
    Physics/*.cpp
    Reflection/*.cpp
    Resources/*.cpp
    Scripting/*.cpp
    Scene/*.cpp
    Utility/*.cpp
    Events/*.cpp
)


add_library(Kos_Engine STATIC ${ENGINE_SOURCE})

# Debug libs
file(GLOB ENGINE_LIBS_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/DebugLibs/*.lib")

# Release libs
file(GLOB ENGINE_LIBS_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/ReleaseLibs/*.lib")

# Link depending on config
target_link_libraries(Kos_Engine PRIVATE
    $<$<CONFIG:Debug>:${ENGINE_LIBS_DEBUG}>
    $<$<CONFIG:Release>:${ENGINE_LIBS_RELEASE}>
)

target_include_directories(Kos_Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    # Treats path as a system-level library
    PUBLIC SYSTEM
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/Include/PHYSX
)

# Precompiled header
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
    target_precompile_headers(Kos_Engine PRIVATE Config/pch.h)
endif()

if(MSVC)
    target_compile_options(Kos_Engine PRIVATE /Zc:preprocessor)
endif()

file(GLOB ENGINE_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/dll/*.dll")
foreach(dll ${ENGINE_DLLS})
    get_filename_component(dll_name "${dll}" NAME)
    add_custom_command(TARGET Kos_Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${dll}"
            "$<TARGET_FILE_DIR:Kos_Engine>/"
        COMMENT "Copying ${dll_name} to output directory"
    )
endforeach()

# # Print debug info (optional)
# message(STATUS "Found ${ENGINE_HEADER} header files")
# message(STATUS "Found ${ENGINE_SOURCE} source files")
# message(STATUS "Found ${ENGINE_LIBS} library files")
# message(STATUS "Output directory: ${OUTPUT_DIR}")
