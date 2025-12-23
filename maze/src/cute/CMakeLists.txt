
add_library(CuteMaze SHARED)

target_sources(CuteMaze PRIVATE
	cuteMaze.cpp
)

target_include_directories(CuteMaze PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(CuteMaze PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../core")

target_link_libraries(CuteMaze
    PRIVATE Maze
    PRIVATE Random
    PRIVATE MathStuff
    PRIVATE cute_lib
)

set_target_properties(CuteMaze PROPERTIES OUTPUT_NAME CuteMaze)
set_target_properties(CuteMaze PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(NOT MSVC)
	set_target_properties(CuteMaze PROPERTIES PREFIX "")
endif()



