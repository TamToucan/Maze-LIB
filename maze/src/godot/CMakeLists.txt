# --- maze/src/godot/CMakeLists.txt ---

add_library(GDMaze SHARED)

target_sources(GDMaze PRIVATE
    GDMaze.cpp
    GDMaze.hpp
    gdextension_registration.cpp
    gdextension_registration.hpp
)

# Includes
target_include_directories(GDMaze PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
# Note: linking 'maze' below usually handles the core include automatically 
# if the core CMake is set up right, but keeping this explicitly is fine/safe.
target_include_directories(GDMaze PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../core")

# Linking
target_link_libraries(GDMaze
    PRIVATE Maze         # The Core Lib
    PRIVATE Random       # From Libs repo
    PRIVATE MathStuff    # From Libs repo
    PUBLIC godot::cpp    # From Godot FetchContent
)

# Properties
set_target_properties(GDMaze PROPERTIES OUTPUT_NAME GDMaze)
set_target_properties(GDMaze PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

# MinGW/Windows cleanup (removes 'lib' prefix)
if(NOT MSVC)
    set_target_properties(GDMaze PROPERTIES PREFIX "")
endif()

# Post-Build Copy
# I used generator expressions ($<TARGET_FILE:...>) which are safer than manual paths.
# They automatically find the .dll/.so regardless of where CMake built it.
add_custom_command(TARGET GDMaze POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:GDMaze>
        "D:/git/the-lott/addons/gdmaze/bin/"
    
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Maze>
        "D:/git/the-lott/addons/gdmaze/bin/"
        
    COMMENT "Copying GDMaze and Core Maze DLLs to Godot project"
)

