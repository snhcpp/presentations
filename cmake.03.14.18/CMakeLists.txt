cmake_minimum_required( VERSION 3.5 )

set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -O0 -g -ggdb ${CMAKE_CXX_EXTRA_FLAGS}" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG ${CMAKE_CXX_EXTRA_FLAGS}" )

message( "** CMAKE_CXX_FLAGS        =${CMAKE_CXX_FLAGS}" )
message( "** CMAKE_CXX_EXTRA_FLAGS  =${CMAKE_CXX_EXTRA_FLAGS}" )
message( "** CMAKE_CXX_FLAGS_DEBUG  =${CMAKE_CXX_FLAGS_DEBUG}" )
message( "** CMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}" )

list( APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CmakeModules )

message( "** CMAKE_MODULE_PATH      =${CMAKE_MODULE_PATH}" )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set( CMAKE_DISABLE_IN_SOURCE_BUILD ON )
set( CMAKE_DISABLE_SOURCE_CHANGES  ON )
set( CMAKE_COLOR_MAKEFILE          ON )

# dump all our binaries at the top. XXX breaks ctest
#set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

if( "${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}" )
    message( SEND_ERROR "In-source builds are not allowed." )
endif()

add_subdirectory( lib/googletest )
add_subdirectory( myproject )
add_subdirectory( ocr )
add_subdirectory( slowmath )
add_subdirectory( calc )
