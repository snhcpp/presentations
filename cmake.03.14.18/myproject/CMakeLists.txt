cmake_minimum_required( VERSION 3.5 )

project( MYPROJECT VERSION 1.0.0 )

add_subdirectory( src )

enable_testing()

add_subdirectory( test )
