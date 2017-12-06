cmake_minimum_required( VERSION 3.5 )

project( ALLOCATOR )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

include_directories( "${PROJECT_SOURCE_DIR}/src" )

add_subdirectory( lib/googletest )
add_subdirectory( src )
add_subdirectory( test )
