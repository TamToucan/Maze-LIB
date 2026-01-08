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

        if(EXISTS "${CUTE_ROOT}/lib/libcute.dll.a")
            message(STATUS "Found Cute DLL Import Library: ${CUTE_ROOT}/lib/libcute.dll.a")
            set(CUTE_FOUND ON)
            add_library(cute SHARED IMPORTED GLOBAL)
            set_target_properties(cute PROPERTIES
                IMPORTED_IMPLIB "${CUTE_ROOT}/lib/libcute.dll.a"
                IMPORTED_LOCATION "${CUTE_ROOT}/bin/libcute.dll"
                INTERFACE_INCLUDE_DIRECTORIES "${CUTE_ROOT}/include;${CUTE_ROOT}"
            )
            add_library(cute_lib ALIAS cute)
        elseif(EXISTS "${CUTE_ROOT}/lib/libcute.a")
            message(STATUS "Found Cute Static Library: ${CUTE_ROOT}/lib/libcute.a")
            set(CUTE_FOUND ON)
            add_library(cute STATIC IMPORTED GLOBAL)
            set_target_properties(cute PROPERTIES
                IMPORTED_LOCATION "${CUTE_ROOT}/lib/libcute.a"
                INTERFACE_INCLUDE_DIRECTORIES "${CUTE_ROOT}/include;${CUTE_ROOT}"
                INTERFACE_COMPILE_DEFINITIONS "CF_STATIC"
            )
            file(GLOB CUTE_DEPENDENCIES "${CUTE_ROOT}/lib/*.a")
            list(REMOVE_ITEM CUTE_DEPENDENCIES "${CUTE_ROOT}/lib/libcute.a")
            set_target_properties(cute PROPERTIES INTERFACE_LINK_LIBRARIES "${CUTE_DEPENDENCIES}")
            add_library(cute_lib ALIAS cute)
        else()
            message(FATAL_ERROR "Framework library not found at ${CUTE_ROOT}/lib/.")
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
    add_subdirectory(maze/src/cute)
endif()
