// clang-format off
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <iostream>

#include "slowmath/slowmath.hpp"
// clang-format on

TEST( SlowMath, Add )
{
  std::cout << "ADDING 1+2 = " << add( 1, 2 ) << std::endl;
}
