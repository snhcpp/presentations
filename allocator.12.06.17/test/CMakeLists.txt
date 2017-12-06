cmake_minimum_required( VERSION 3.5 )

include( AddCxxExecutable )

add_cxx11_executable( BUILD_TARGET skeleton_allocator_test
                      SOURCE_LIST skeleton_allocator_test.cpp )

add_cxx11_executable( BUILD_TARGET counting_allocator_test
                      SOURCE_LIST counting_allocator_test.cpp )

add_cxx11_executable( BUILD_TARGET generic_counting_allocator_test
                      SOURCE_LIST generic_counting_allocator_test.cpp )

include( CTest )

enable_testing()

add_test( skeleton_test skeleton_allocator_test )
add_test( counting_test counting_allocator_test )
add_test( generic_counting_test generic_counting_allocator_test )
