cmake_minimum_required(VERSION 3.5)
project(SCRIPTS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/Script")



file(GLOB_RECURSE EDITOR_SOURCES "*.cpp")

# Build a DLL instead of an executable
add_library(SCRIPTS SHARED ${EDITOR_SOURCES})

# Link against your engine library
target_link_libraries(SCRIPTS PRIVATE Kos_Engine)

if(MSVC)
    target_compile_options(SCRIPTS PRIVATE /Zc:preprocessor /FS)
    set_target_properties(SCRIPTS PROPERTIES COMPILE_PDB_NAME SCRIPTS)
endif()

target_include_directories(SCRIPTS
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)


# Output DLL to a temporary location
set_target_properties(SCRIPTS PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/SCRIPTS"
    PDB_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/SCRIPTS"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/SCRIPTS"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/SCRIPTS"
)

if(NOT HOT_RELOAD_BUILD)
    add_custom_command(TARGET SCRIPTS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SCRIPTS>
            "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/script.dll"
    )
endif()

# Post-build step to copy DLL next to your engine/game exe
#option(SKIP_POSTBUILD "Skip postbuild steps" OFF)

#if(NOT SKIP_POSTBUILD)
 #   add_custom_command(TARGET SCRIPTS POST_BUILD
 #   COMMAND ${CMAKE_COMMAND} -E copy_if_different
  #      "$<TARGET_FILE:SCRIPTS>"           # DLL
  #      "${OUTPUT_DIR}"
  #  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  #      "$<TARGET_PDB_FILE:SCRIPTS>"       # PDB
  #     "${OUTPUT_DIR}"
   # COMMENT "Copying SCRIPTS.dll and SCRIPTS.pdb to ${OUTPUT_DIR}"
  #  )
#endif()



