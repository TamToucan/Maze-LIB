# 1. Define the Core Library
# We use SHARED to ensure it works as a DLL/So, beneficial for Godot GDExtension later
add_library(maze SHARED)

# 2. Add Sources
# Kept your .C extension (Standard C++), but ensure your IDE treats them as C++
target_sources(maze PRIVATE
    CellLoc.C
    CellLoc.h
    CellType.h
    Generator.C
    Generator.h
    MazeData.C
    MazeData.h
    MazeHelper.C
    MazeHelper.h
    Node.C
    Node.h
    TileData.C
    TileData.h
)

# 3. Include Directories
# This adds the current folder to the include path so you can #include "MazeData.h" directly
target_include_directories(maze PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include> 
)

# 4. Link Dependencies
# These targets (Random, MathStuff) must be provided by the 'Libs' repo 
# which is fetched by your ROOT CMake file.
target_link_libraries(maze 
    PRIVATE Random 
    PRIVATE MathStuff
)

# 5. Windows / MinGW Specific Settings
set_target_properties(maze PROPERTIES 
    OUTPUT_NAME "maze"
    WINDOWS_EXPORT_ALL_SYMBOLS ON  # Critical for MinGW shared libraries
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Remove the "lib" prefix on Windows (optional, personal preference)
if(MINGW OR MSVC)
    set_target_properties(maze PROPERTIES PREFIX "")
endif()
