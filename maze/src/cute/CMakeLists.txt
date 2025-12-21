
add_library(CuteMaze SHARED)

target_sources(CuteMaze PRIVATE
	CuteMaze.cpp
	CuteMaze.hpp
)

target_include_directories(CuteMaze PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(CuteMaze PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../core")

target_link_libraries(CuteMaze
    PRIVATE maze
    PRIVATE Random
    PRIVATE MathStuff
)

set_target_properties(CuteMaze PROPERTIES OUTPUT_NAME CuteMaze)
set_target_properties(CuteMaze PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(NOT MSVC)
	set_target_properties(CuteMaze PROPERTIES PREFIX "")
endif()



