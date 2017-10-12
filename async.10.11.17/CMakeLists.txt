project(async_demo)

cmake_minimum_required(VERSION 3.5)

# provides compile_commands.json
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

include(CTest)

add_subdirectory(lib/googletest)
#add_subdirectory(lib/cryptopp)

add_executable(demo demo.cpp)

# gets you c++14
target_compile_features(demo PRIVATE cxx_lambda_init_captures)

target_link_libraries(demo gtest gmock_main) #cryptopp-shared)

enable_testing()

add_test(demo_test demo)
