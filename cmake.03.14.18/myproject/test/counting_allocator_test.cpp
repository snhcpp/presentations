// SNHCPP Exploring std::allocator
// https://www.meetup.com/preview/Nashua-C-C-Meetup
// author: Peter M. Petrakis <peter.petrakis@gmail.com>
// no license, do what you want

// resources:
// # last C++11 working standard before you have to pay for it:
// -- allocator section start 17.6.3.5
// -- http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3242.pdf

// # good allocator boilerplate that follows the standard
// -- https://howardhinnant.github.io/allocator_boilerplate.html
//
// # great slide deck that walks through allocator specific
// https://accu.org/content/conf2012/JonathanWakely-CXX11_allocators.pdf
//
// # C++17 How to write custom allocators
// -- https://www.youtube.com/watch?v=kSWfushlvB8
// -- "We've all heard heroic tales of other people this" - John McFarlane CppCon 2017
// -- while focused on C++17, it does a survey from the beginning and explains
//    all implementations. the C++17 portion is at the very very end of the presentation.
//    by far, the best allocator presentation I have found, better than the bloomberg ones.
// ## interesting time marks
// -- 28:20 demonstrates how an allocator is used in a container
// -- 29:00 alloctor_traits interface (you need to use pointer_traits too btw)
// -- 44:00 a minimal allocator
// -- 46:00 C++17 Polymorphic memory resources (PMR)
// -- 52:00 a container's point of view
// -- 54:57 POCCA
// -- 55:26 POCMA
// -- 57:27 POCS
// -- 1:00:00 traditional allocator implementation strategy
// -- 1:02:00 POC.. guidelines (huge!)
// -- 1:03:00 PMR allocator implementation strategy
//
// # great explination of how propogate on... works
// -- https://stackoverflow.com/questions/40801678/how-is-allocator-aware-container-assignment-implemented
//
// clang-format off
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// for native thread_id
#include <sys/types.h>
#include <sys/syscall.h>

#include <cstddef>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <functional>
#include <numeric>
#include <memory>

#include "helpers.hpp"
#include "counting_allocator.hpp"
// clang-format on

TEST( Counter, Baseline )
{
  using allocator_t = counting_allocator<int>;
  using container_t = std::vector<int, allocator_t>;

  container_t v;

  // remember, shared context
  auto v_a = v.get_allocator();

  std::cout << Inspect::show( v_a ) << std::endl;

  v.push_back(10);
  EXPECT_TRUE( v_a.count() == 4 );

  v.push_back(20);
  EXPECT_TRUE( v_a.count() == 8 );

  v.push_back(30);
  EXPECT_TRUE( v_a.count() == 16 );

  dump_container(v);

  std::cout << "count: " << v_a.count() << std::endl;

  std::cout << "...scaling\n";
  auto other = scale_container(v);
  //auto other = shared_scale_container(v);
  //auto other = copy_scale_container(v);
  std::cout << "...scaling return\n";
  const auto scale_count = other.get_allocator().count();

  std::cout << "returned vector from scale call count: " << scale_count << std::endl;

  // reserving space in advanced is more efficient
  EXPECT_TRUE( other.get_allocator().count() == 12 );
  dump_container(other);

  std::cout << "copy assign from scaled container\n";

  // PPOCA
  container_t third = other;
  auto third_count = third.get_allocator().count();
  std::cout << "PPOCA count: " << third_count << std::endl;

  // it's a running total of all bytes provisioned
  EXPECT_TRUE( third_count == 2 * scale_count );
}
