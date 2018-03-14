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
#include "generic_counting_allocator.hpp"
// clang-format on

TEST( Example, BuildOut )
{
  using allocator_type = BaseAllocator< testy, CountingAllocatorPolicy >;
  allocator_type a1;

  {
    allocator_type aa;
    (void)( a1 == aa );
    (void)( a1 != aa );
  }

  std::cout << "======================" << std::endl;

  std::cout << " allocate 20 testy \n";
  testy* p1 = a1.allocate( 20 );

  std::cout << "construct 20 testy \n";
  for ( size_t i = 0; i < 20; ++i )
  {
    a1.construct( p1 + i, i * 7 );
  }

  decltype(a1)::policy_type::report( a1.get_policy_storage() );

  std::cout << "destroy 20 testy \n";
  for ( size_t i = 0; i < 20; ++i )
  {
    a1.destroy( p1 + i );
  }

  std::cout << " deallocate 20 testy \n";
  a1.deallocate( p1, 20 );

  std::cout << "======================" << std::endl;

  // default allocator for strings
  std::allocator<std::string> a2;

  std::cout << "rebind test" << std::endl;

  // same, but obtained by rebinding from the type of a1
  decltype( a1 )::rebind< std::string >::other a2_1;

  // same, but obtained by rebinding from the type of a1 via allocator_traits
  std::allocator_traits< decltype( a1 ) >::rebind_alloc< std::string > a2_2;

  std::string* s = a2.allocate(2); // space for 2 strings

  a2.construct( s, "foo" );
  a2.construct( s + 1, "bar" );

  std::cout << s[0] << ' ' << s[1] << '\n';

  a2.destroy( s );
  a2.destroy( s + 1 );
  a2.deallocate( s, 2 );

#if 0
  std::cout << "======================" << std::endl;
  std::cout << "provision a vector\n";
  std::vector< int, BaseAllocator< int, CountingAllocatorPolicy > > array(100);

  for ( size_t i = 0; i < array.capacity(); ++i )
  {
    std::cout << "iter " << i << std::endl;
    array[i] = i * 7;
    if ( ( i % 10 ) == 0 )
    {
      decltype( array )::allocator_type::policy_type::report(
          ( array.get_allocator() ).get_policy_storage() );
    }
  }

  for ( const auto& value : array )
  {
    std::cout << value << ",";
  }
  std::cout << std::endl;
#endif
}

