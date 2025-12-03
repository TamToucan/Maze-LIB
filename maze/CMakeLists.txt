project(Maze-Lib)
SET(LIB_NAME GDMaze) 

# Automatically pick up files added to src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.C"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
        
add_library(${LIB_NAME} SHARED ${SOURCES})

set_target_properties(${LIB_NAME} PROPERTIES OUTPUT_NAME ${LIB_NAME})
if(NOT MSVC)
	set_target_properties(${LIB_NAME} PROPERTIES PREFIX "")
endif()


target_include_directories(${LIB_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(${LIB_NAME}
	PRIVATE Random
	PRIVATE MathStuff
	PUBLIC godot::cpp)

add_custom_command(
        TARGET ${LIB_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:${LIB_NAME}>
			    "D:/git/the-lott/addons/gdmaze/bin")
