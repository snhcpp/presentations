# basic assignment

# NOTE the '\n' are not necessary, adding for output clarity

set( MYVAR "hello world" )

# basic output
message( "I said... ${MYVAR} \n" )

# unassignment
unset( MYVAR )
message( "I said... ${MYVAR} \n" )

# everything is a string, you can deref
# strings and create variable names
set( CONTROL LIGHT )
set( ${CONTROL}_ON YES )
message( "composed var LIGHT_ON: ${LIGHT_ON} \n" )

# get an env var from host
message( "TERM is $ENV{TERM}" )

# same thing, deref it and assign it to a var
set( TERM_VAR $ENV{TERM} )
message( "TERM is ${TERM_VAR}" )

# set env var in this process
set( ENV{TERM} "remember-rxvt!" )
message( "TERM is $ENV{TERM}" )
