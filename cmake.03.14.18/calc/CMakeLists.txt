project( CALC VERSION 1.0.0 )

add_executable( calc main.cpp )

target_compile_features( calc PRIVATE cxx_lambdas)

target_link_libraries( calc libslowmath )
