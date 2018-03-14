project( SlowMath VERSION 1.0.0 )

add_subdirectory( src )

add_library( ${PROJECT_NAME}  src/operations.cpp )

add_library( libslowmath ALIAS ${PROJECT_NAME} )

target_compile_features( ${PROJECT_NAME} PRIVATE cxx_lambdas)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

enable_testing()

add_subdirectory( test )
