# sets the following in PARENT_SCOPE
#
# Tesseract_FOUND        - Boolean
# Tesseract_INCLUDE_DIRS - the Tesseract include directories
# Tesseract_LIBRARIES    - link these to use Tesseract

include(LibFindMacros)

find_path(Tesseract_INC
  NAMES tesseract/baseapi.h
  HINTS "/usr/include"
        "/usr/include/tesseract"
        "/usr/local/include"
        "/usr/local/include/tesseract"
        ${Tesseract_PKGCONF_INCLUDE_DIRS})

find_library(Tesseract_LIB
  NAMES tesseract libtesseract libtesseract tesseract-static
  HINTS "/usr/lib"
        "/usr/local/lib"
        ${Tesseract_PKGCONF_LIBRARY_DIRS})

set(Tesseract_PROCESS_INCLUDES Tesseract_INC)
set(Tesseract_PROCESS_LIBS Tesseract_LIB)

libfind_process(Tesseract)

#message("XXX Tesseract_INCLUDE_DIRS ${Tesseract_INCLUDE_DIRS}")
#message("XXX Tesseract_LIBRARIES    ${Tesseract_LIBRARIES}")
