cmake_minimum_required(VERSION 3.20)

if(DEFINED BUILD_GODOT)
    set(BUILD_GODOT_MAZE ${BUILD_GODOT})
else()
    set(BUILD_GODOT_MAZE ON)
endif()

if (NOT ALL_SUBS_BUILD_ON)
    set(FETCHCONTENT_QUIET OFF)
    project(Maze-Lib)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    
    set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)
    
    include(FetchContent)
    
    if(BUILD_GODOT_MAZE)
        FetchContent_Declare(
                GDExtension
                GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
                GIT_TAG godot-4.3-stable
        )
    endif()
    
    FetchContent_Declare(
        Libs
        GIT_REPOSITORY https://github.com/TamToucan/Libs.git
        GIT_TAG main
    )
    
    if(BUILD_GODOT_MAZE)
        FetchContent_MakeAvailable(GDExtension Libs)
    else()
        FetchContent_MakeAvailable(Libs)
    endif()
else()
	message(STATUS "Maze-LIB is being built as part of Lott-LIB, skipping FetchContent.")
endif()
    
add_subdirectory(maze)
