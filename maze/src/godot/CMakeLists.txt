
add_library(GDMaze SHARED)

target_sources(GDMaze PRIVATE
    GDMaze.cpp
    GDMaze.hpp
    gdextension_registration.cpp
    gdextension_registration.hpp
)

target_include_directories(GDMaze PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(GDMaze PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../core")

target_link_libraries(GDMaze
    PRIVATE maze
    PRIVATE Random
    PRIVATE MathStuff
    PUBLIC godot::cpp
)

set_target_properties(GDMaze PROPERTIES OUTPUT_NAME GDMaze)
if(NOT MSVC)
	set_target_properties(GDMaze PROPERTIES PREFIX "")
endif()

add_custom_command(
        TARGET GDMaze  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:GDMaze>
			    "D:/git/the-lott/addons/gdmaze/bin"
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/../core/$<TARGET_FILE_NAME:maze>
			    "D:/git/the-lott/addons/gdmaze/bin")



