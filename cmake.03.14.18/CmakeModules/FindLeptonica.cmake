# sets the following in PARENT_SCOPE
#
#  Leptonica_FOUND        - Boolean
#  Leptonica_INCLUDE_DIRS - the Leptonica include directories
#  Leptonica_LIBRARIES    - link these to use Leptonica

include(LibFindMacros)

find_path(Leptonica_INC
  NAMES leptonica/allheaders.h
  HINTS ${LEP_INCLUDE_SEARCH_PATHS})

set(Leptonica_PROCESS_INCLUDES Leptonica_INC)

find_library(Leptonica_LIB
  NAMES leptonica libleptonica liblept lept
  HINTS "/usr/lib"
        "/usr/local/lib"
        ${Leptonica_PKGCONF_LIBRARY_DIRS})

set(Leptonica_PROCESS_LIBS Leptonica_LIB)

libfind_process(Leptonica)

#message("XXX Leptonica_INCLUDE_DIRS ${Leptonica_INCLUDE_DIRS}")
#message("XXX Leptonica_LIBRARIES    ${Leptonica_LIBRARIES}")
