cmake_minimum_required(VERSION 3.20)

# --- STANDALONE SETUP START ---
if(NOT ALL_SUBS_BUILD_ON)
    project(Maze-LIB)
    
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    
    option(BUILD_GODOT "Build with Godot support" OFF)
    option(BUILD_CUTE "Build with Cute Framework support" ON)

    include(FetchContent)

    # 1. Fetch Common Libs
    FetchContent_Declare(
        Libs
        GIT_REPOSITORY https://github.com/TamToucan/Libs.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(Libs)

    # 2. Setup Godot
    if(BUILD_GODOT)
        FetchContent_Declare(
            GDExtension
            GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
            GIT_TAG godot-4.3-stable
        )
        FetchContent_MakeAvailable(GDExtension)
    endif()

    # 3. Setup Cute (FIXED)
    if(BUILD_CUTE)
        set(CUTE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/deps/cute")

        # Check for the main static library file
        if(EXISTS "${CUTE_ROOT}/lib/libcute.a")
            set(CUTE_FOUND ON)

            # Define 'cute' as a pre-built static library
            add_library(cute STATIC IMPORTED GLOBAL)

            # Set the location of the main library and headers
            set_target_properties(cute PROPERTIES
                IMPORTED_LOCATION "${CUTE_ROOT}/lib/libcute.a"
                INTERFACE_INCLUDE_DIRECTORIES "${CUTE_ROOT}/include;${CUTE_ROOT}"
            )

            # --- HANDLE STATIC DEPENDENCIES (PhysFS, SPIRV) ---
            # Static libs don't link their own deps automatically. We must explicitly 
            # tell CMake that linking 'cute' requires linking the other .a files in lib/.
            file(GLOB CUTE_DEPENDENCIES "${CUTE_ROOT}/lib/*.a")
            
            # Remove libcute.a from this list to avoid circular linking warnings
            list(REMOVE_ITEM CUTE_DEPENDENCIES "${CUTE_ROOT}/lib/libcute.a")

            # Attach these dependencies to the 'cute' target
            set_target_properties(cute PROPERTIES INTERFACE_LINK_LIBRARIES "${CUTE_DEPENDENCIES}")

            message(STATUS "Found Cute Framework: ${CUTE_ROOT}")
        else()
            message(FATAL_ERROR "Static library libcute.a not found at ${CUTE_ROOT}/lib/.\nPlease run setup.sh or ensure you have built the dependencies.")
        endif()
    endif()
endif()
# --- STANDALONE SETUP END ---


# --- 1. Core Maze Target ---
# Points to maze/src/core/CMakeLists.txt
# Assumes this creates a target named 'maze'
add_subdirectory(maze/src/core) 
add_library(MazeLib::Maze ALIAS Maze)


# --- 2. Godot Target (GDExtension) ---
if(BUILD_GODOT)
    add_subdirectory(maze/src/godot)
endif()


# --- 3. Cute Framework Target ---
if(BUILD_CUTE AND CUTE_FOUND)
    # Define the library for the Cute wrapper
    add_library(CuteMaze SHARED maze/src/cute/cuteMaze.cpp)
    
    # Link against the Core Maze logic and the imported 'cute' target
    target_link_libraries(CuteMaze PRIVATE Maze cute)
    
    # Ensure this library can find its own headers
    target_include_directories(CuteMaze PRIVATE src/cute)
endif()
