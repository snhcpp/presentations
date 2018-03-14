cmake_minimum_required( VERSION 3.5 )

add_executable( add_test add_test.cpp  )

target_compile_features( add_test
  PRIVATE
    cxx_strong_enums
    cxx_lambdas
    cxx_range_for
)

target_link_libraries( add_test
  gtest
  gmock_main
  libslowmath
)

add_test( SlowMath.add  add_test )
