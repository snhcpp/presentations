# lists, note the lack of quotations
# cmake automagically appends a ';' to each bareword which is
# it's list delimiter

message( "building MYLIST" )
set( MYLIST A B C D E )
# -- stored as "A;B;C;D;E"

message( "iterating MYLIST" )
foreach( INDEX ${MYLIST} )
  message( " element: ${INDEX} " )
endforeach()

message( "integer for loop range" )

# simple for loop with numerical range
foreach( INDEX RANGE 0 10 )
  message( " segment ${INDEX} " )
endforeach()

message( "========= list operations =============" )
# but var constructed lists can be operated on by this directive too
LIST( LENGTH MYLIST MYLIST_SIZE )

# counts from zero
message( "mylist length ${MYLIST_SIZE}" )

LIST( APPEND MYLIST HELLO )

LIST( GET MYLIST 5 ML_6 )

message( "while loops" )

# this should upset you
SET( IDX 0 )
while( ${IDX} LESS ${MYLIST_SIZE} )
  LIST( GET MYLIST ${IDX} ELE )
  message(" index ${IDX}: ${ELE}")
  MATH( EXPR IDX "${IDX} + 1" )
endwhile()
