cmake_minimum_required( VERSION 3.5 )

include( AddCxxExecutable )

find_package(Tesseract REQUIRED)

if (NOT ${Tesseract_FOUND})
  message (FATAL_ERROR "Tesseract not found!")
endif()

find_package(Leptonica REQUIRED)

if (NOT ${Leptonica_FOUND})
  message (FATAL_ERROR "Leptonica not found!")
endif()

include_directories( "${OCR_SOURCE_DIR}/src" )
include_directories( "${Tesseract_INCLUDE_DIRS}" )
include_directories( "${Leptonica_INCLUDE_DIRS}" )

add_executable( tesseract_test tesseract_test.cpp  )

target_compile_features( tesseract_test
  PRIVATE
    cxx_strong_enums
    cxx_lambdas
    cxx_range_for
)

target_link_libraries( tesseract_test
  gtest
  gmock_main
  ${Tesseract_LIBRARIES}
  ${Leptonica_LIBRARIES}
)

#
# provide image file for unit test
#
add_custom_command(TARGET tesseract_test
                   POST_BUILD
                   COMMAND cp ${OCR_SOURCE_DIR}/test/phototest.tif ${CMAKE_BINARY_DIR}/ocr/test
                   COMMENT "copying phototest.tif"
                   )

add_test( ocr.smoketest  tesseract_test )
