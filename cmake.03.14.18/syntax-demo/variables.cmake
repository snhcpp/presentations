
#
# everything is a command e.g. if() ... endif()
#
# everything is a string
#

# basic assignment
set( MYVAR "hello world" )

# basic output
message( " I said... ${MYVAR} \n" )

# unassignment
unset( MYVAR )
message( " I said... ${MYVAR} \n" )

# get an env var from host
message(  " TERM is $ENV{TERM}" )

# same thing, deref it and assign it to a var
set( TERM_VAR "$ENV{TERM}" )
message( " TERM is ${TERM_VAR}" )

# set env var in this process
set( ENV{TERM} "remember-rxvt!" )
message(  " TERM is $ENV{TERM}" )

# stop cmake in it's tracks
##message( FATAL_ERROR " we die here" )

# lists, note the lack of quotations
# cmake automagically appends a ';' to each bareword which is
# it's list delimiter
set( MYLIST A B C D E )
# -- stored as "A;B;C;D;E"

foreach( INDEX ${MYLIST} )
  message( " segment ${INDEX} " )
endforeach()

message( "===========================" )

# simple for loop with numerical range
foreach( INDEX RANGE 0 10 )
  message( " segment ${INDEX} " )
endforeach()


message( "========= list operations =============" )
# actual list type. NOTE, no "create"
LIST( APPEND REAL_LIST 1 2 3 4 5 )

# but var constructed lists can be operated on by this directive too
LIST( LENGTH MYLIST MYLIST_SIZE )

# counts from zero
message( "mylist length ${MYLIST_SIZE}" )

LIST( APPEND MYLIST HELLO )

LIST( GET MYLIST 5 ML_6 )

message( "new list element ${ML_6}" )

# but if everything is a string? how does cmake differentiate between
# lists and plain old strings you want to operate on? It doesn't.

message( "========= string operations =============" )

string( APPEND MYLIST " world")
message( " list or string? ${MYLIST}" )
# output =>  list or string? A;B;C;D;E;HELLO world

# keep it simple for me...
set( MYPATH "/home/ppetraki" )
string( APPEND MYPATH "/Desktop" )
message( "home dir path - ${MYPATH}" )

# if/else. note the () even for else and endif
if( ${MYPATH} STREQUAL "/home/ppetraki" )
  message( "that's not right" )
else()
  message( "MYPATH DNE /home/ppetraki" )
endif()

# XXX macros/functions
