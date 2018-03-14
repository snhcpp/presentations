
#https://cmake.org/cmake/help/v3.0/command/function.html

# functions operate with own scope and no side effects

function(add_two A B)
	math(EXPR RES "${A} + ${B}")
	message( "res ${RES}" )
endfunction()

add_two(1 2)
message( "add_two_res local scope var: (${RES})" )


# how do I return a value?

set( RES "ERROR" )
function( add_two_ref RES A B )
	math(EXPR _RES "${A} + ${B}")
	set(RES ${_RES} PARENT_SCOPE)
endfunction()

add_two_ref(RES 1 2)
message( "reference res ${RES}" )

#[[ In addition to referencing the formal parameters you can reference the variable ARGC which will be set to the number of arguments passed into the function as well as ARGV0 ARGV1 ARGV2 ... which will have the actual values of the arguments passed in. This facilitates creating functions with optional arguments. Additionally ARGV holds the list of all arguments given to the function and ARGN holds the list of arguments past the last expected argument.]]

function(accumulate)
	set(SUM 0)
	set(IDX 1)

	while( ${IDX} LESS ${ARGC} )
		LIST(GET ARGN ${IDX} EL)
		MATH(EXPR SUM "${SUM} + ${EL}")
		MATH(EXPR IDX "${IDX} + 1")
	endwhile()

	# we're writing back to the variable name stored in ARGV0
	set(${ARGV0} ${SUM} PARENT_SCOPE)
endfunction()

set (accumulate_res "ERROR")
accumulate(accumulate_res 1 2 3 4 5 6)

message("accumulate (${accumulate_res})")

#https://cmake.org/cmake/help/v3.0/command/macro.html

# usage find_library(ProtoBuf)
# side effect sets the following variables:
# PROTOBUF_LIBRARY
# PROTOBUF_INCLUDE_DIR
macro(find_library LIB)
    set( FL_LIB_NAME "" )
    string( TOUPPER ${LIB} FL_LIB_NAME )
    set( ${FL_LIB_NAME}_LIBRARY  "[macro]/path/to/some.so" )
    set( ${FL_LIB_NAME}_INCLUDE_DIR  "[macro]/usr/include/google" )
endmacro()

find_library(Glog)

message("Glog lib ${GLOG_LIBRARY}")
message("Glog include dir ${GLOG_INCLUDE_DIR}")

# those local vars are still accessible
message("to upper name (${FL_LIB_NAME})")

# same sort of style with functions
function(find_library2 LIB)
    set( FL2_LIB_NAME "" )
    string( TOUPPER ${LIB} FL_LIB_NAME )
    set( ${FL_LIB_NAME}_LIBRARY  "[function]/path/to/some.so" PARENT_SCOPE )
    set( ${FL_LIB_NAME}_INCLUDE_DIR  "[function]/usr/include/google" PARENT_SCOPE )
endfunction()

find_library2(Protobuf)

message("PB lib ${PROTOBUF_LIBRARY}")
message("PB include dir ${PROTOBUF_INCLUDE_DIR}")

message("to upper name (${FL2_LIB_NAME})")
