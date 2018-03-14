# https://cmake.org/cmake/help/v3.4/command/if.html

# if/else. note the () even for else and endif
if( $ENV{HOME} STREQUAL "/home/ppetraki" )
  message( "that's my home, $ENV{HOME}" )
else()
  message( "that's not my home!" )
endif()

# booleans have caveats
#
# https://cmake.org/Wiki/CMake/Language_Syntax#CMake_supports_boolean_variables.
#[[
CMake considers an empty string, "FALSE", "OFF", "NO", or any string ending in "-NOTFOUND" to be false. (This happens to be case-insensitive, so "False", "off", "no", and "something-NotFound" are all false.) Other values are true. Thus it matters not whether you use TRUE and FALSE, ON and OFF, or YES and NO for your booleans.

Many scripts expect a string to either be false or contain a useful value, often a path to a directory or a file. You have a potential but rare problem if one of the useful values coincides with falseness. Avoid giving nonsensical names like /tmp/ME-NOTFOUND to your files, executables or libraries.]]

# cmake --help-policy CMP0012
cmake_policy( SET CMP0012 NEW )

if (TRUE)
    message( "true statement" )
endif()

if (TRUE AND BAZ)
    message( "not a true statement" )
endif()

if ( NOT BAZ-NOTFOUND )
    message( "baz not found!" )
endif()
