include(CMakeParseArguments)

macro( DUMP_ALL )
	get_cmake_property(_variableNames VARIABLES)
	foreach (_variableName ${_variableNames})
			message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()
endmacro()

function( ADD_CXX_EXECUTABLE_IMPL )
  unset( options )
  set( oneValueArgs  BUILD_TARGET  BUILD_TARGET_
                     CXX_FEATURE CXX_FEATURE_ )

  set( multiValueArgs SOURCE_LIST  SOURCE_LIST_  )

  cmake_parse_arguments( ADD_CXX_EXECUTABLE_IMPL
                         "${options}"
                         "${oneValueArgs}"
                         "${multiValueArgs}"
                         ${ARGN} )

  add_executable( "${ADD_CXX_EXECUTABLE_IMPL_BUILD_TARGET}"
                  "${ADD_CXX_EXECUTABLE_IMPL_SOURCE_LIST}" )

  target_compile_features( ${ADD_CXX_EXECUTABLE_IMPL_BUILD_TARGET}
                           PRIVATE ${ADD_CXX_EXECUTABLE_IMPL_CXX_FEATURE} )

  target_link_libraries( ${ADD_CXX_EXECUTABLE_IMPL_BUILD_TARGET}  gtest gmock_main )
endfunction()

function( ADD_CXX11_EXECUTABLE )
  unset( options )
  set( oneValueArgs  BUILD_TARGET  BUILD_TARGET_ )
  set( multiValueArgs SOURCE_LIST  SOURCE_LIST_  )

  cmake_parse_arguments( ADD_CXX11_EXECUTABLE
                         "${options}"
                         "${oneValueArgs}"
                         "${multiValueArgs}"
                         ${ARGN} )

  add_cxx_executable_impl( BUILD_TARGET ${ADD_CXX11_EXECUTABLE_BUILD_TARGET}
                           CXX_FEATURE cxx_range_for
                           SOURCE_LIST ${ADD_CXX11_EXECUTABLE_SOURCE_LIST}
                         )
endfunction()

function( ADD_CXX14_EXECUTABLE )
  unset( options )
  set( oneValueArgs  BUILD_TARGET  BUILD_TARGET_ )
  set( multiValueArgs SOURCE_LIST  SOURCE_LIST_  )

  cmake_parse_arguments( ADD_CXX14_EXECUTABLE
                         "${options}"
                         "${oneValueArgs}"
                         "${multiValueArgs}"
                         ${ARGN} )

  add_cxx_executable_impl( BUILD_TARGET ${ADD_CXX14_EXECUTABLE_BUILD_TARGET}
                           CXX_FEATURE cxx_lambda_init_captures
                           SOURCE_LIST ${ADD_CXX14_EXECUTABLE_SOURCE_LIST}
                         )
endfunction()
