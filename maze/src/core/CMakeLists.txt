
add_library(maze SHARED)

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

target_include_directories(maze PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(maze
	PRIVATE Random
	PRIVATE MathStuff
)

set_target_properties(maze PROPERTIES OUTPUT_NAME maze)
set_target_properties(maze PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(NOT MSVC)
	set_target_properties(maze PROPERTIES PREFIX "")
endif()
