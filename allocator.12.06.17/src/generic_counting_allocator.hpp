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
// additional resources:
// -- http://en.cppreference.com/w/cpp/memory/allocator
// -- http://en.cppreference.com/w/cpp/memory/allocator_traits
// -- https://stackoverflow.com/questions/872675/policy-based-design-and-best-practices-c
// -- https://stackoverflow.com/questions/14718055/what-is-the-difference-between-a-trait-and-a-policy
// -- https://stackoverflow.com/questions/19803954/template-template-parameters-and-clang
// -- https://codereview.stackexchange.com/questions/31528/a-working-stack-allocator
// -- https://stackoverflow.com/questions/826569/compelling-examples-of-custom-c-allocators#826635

#pragma once

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
// provides allocator_traits
#include <memory>
// clang-format on

template < typename T >
class CountingAllocatorPolicy
{
public:
  using value_type      = T;
  using storage_type    = size_t;
  using storage_pointer = size_t*;

  static constexpr size_t ObjectSize = sizeof( T );

  // since we're using statics we must allocate storage outside
  // of the class and pass it to each member function.
  static storage_pointer create()
  {
    std::cout << "creating CountingAllocatorPolicy storage for object size: " << ObjectSize
              << std::endl;
    storage_pointer p = new storage_type;
    *p                = 0;
    return p;
  }

  // our destructor so to speak and where we do our reporting, for now.
  static void destroy( storage_pointer p )
  {
    std::cout << "destroying CountingAllocatorPolicy storage for object size: " << ObjectSize
              << std::endl;
    std::cout << "ACCOUNTING: " << *p << " objects with size " << ObjectSize << std::endl;
    delete p;
  }

  static void count( size_t n, storage_pointer p )
  {
    std::cout << "adding " << n << " bytes\n";
    ( *p ) += n;
  }

  static void uncount( size_t n, storage_pointer p )
  {
    std::cout << "subtracting " << n << " bytes\n";
    ( *p ) -= n;
  }

  static void report(const storage_pointer p)
  {
    std::cout << "report: " << *p << " objects with size " << ObjectSize << std::endl;
  }

  static bool equals( const storage_pointer p1, const storage_pointer p2 )
  {
    return *p1 == *p2;
  }

  static bool not_equals( const storage_pointer p1, const storage_pointer p2 )
  {
    return *p1 != *p2;
  }

}; // CountingAllocatorPolicy

// C++11
template < typename T, template < typename > class TrackingPolicy >
class BaseAllocator
{
public:
  // unspecialized policy type for later use by rebind
  template < typename U >
  using POLICY = TrackingPolicy< U >;

  // specialized policy types
  using policy_type            = POLICY< T >;
  using policy_storage_pointer = typename POLICY< T >::storage_pointer;

  // allocator interface types
  using allocator_type     = BaseAllocator< T, TrackingPolicy >;
  using value_type         = T;
  using pointer            = T*; // don't need this and the rest in C++17
  using const_pointer      = const T*;
  using void_pointer       = void*;
  using const_void_pointer = const void*;
  using reference          = T&;
  using const_reference    = const T&;
  using size_type          = std::size_t;
  using difference_type    = std::ptrdiff_t;

  // C++11 only
  template < typename V >
  struct rebind
  {
    using other = BaseAllocator< V, POLICY >;
  };

  BaseAllocator() noexcept
  {
    m_policy_storage_p = TrackingPolicy< T >::create();
  }

  ~BaseAllocator()
  {
    TrackingPolicy< T >::destroy( m_policy_storage_p );
  }

  template < typename Y, template < typename > class TP >
  BaseAllocator( const BaseAllocator< Y, TP >& other )
  {
    //*m_policy_storage_p = *other.get_policy_storage();
  }

  pointer allocate( size_type n, const_void_pointer hint = 0 )
  {
    (void)hint;
    pointer p = static_cast< pointer >( operator new( sizeof( value_type ) * n ) );
    TrackingPolicy< T >::count( n, m_policy_storage_p );
    return p;
  }

  void deallocate( T* p, std::size_t n )
  {
    (void)n;
    operator delete( p );
    // TrackingPolicy< T >::uncount( n, m_policy_storage_p );
    // doesn't mention any required exceptions to be
    // thrown. But it's not marked noexcept either.
  }

  // C++11 only
  size_type max_size() const
  {
    return std::numeric_limits< size_type >::max() / sizeof( value_type );
  }

  // C++11 only
  // does placement new in allocated storage
  template < class U, class... Args >
  void construct( U* p, Args&&... args )
  {
    ::new ( (void*)p ) U( std::forward< Args >( args )... );
  }

  // C++11 only
  // calls destructor on placement new allocated storage
  template < class U >
  void destroy( U* p )
  {
    p->~U();
  }

  bool equals( const allocator_type& rhs ) const
  {
    return policy_type::equals( m_policy_storage_p, rhs.get_policy_storage() );
  }

  bool not_equals( const allocator_type& rhs ) const
  {
    return policy_type::not_equals( m_policy_storage_p, rhs.get_policy_storage() );
  }

  policy_storage_pointer get_policy_storage() const
  {
    return m_policy_storage_p;
  }

private:
  policy_storage_pointer m_policy_storage_p;
}; // class BaseAllocator


template < typename ValueType >
using CountingAllocator = BaseAllocator< ValueType, CountingAllocatorPolicy >;

template < typename T1, typename T2 >
bool operator==( CountingAllocator< T1 >& lhs, CountingAllocator< T2 >& rhs )
{
  return lhs.equals( rhs );
}

template < typename T1, typename T2 >
bool operator!=( CountingAllocator< T1 >& lhs, CountingAllocator< T2 >& rhs )
{
  return lhs.not_equals( rhs );
}

class testy
{
public:
  testy()
  {
    value = 717;
  }

  testy( int x )
      : value{x}
  {
    std::cout << "testy constructor firing " << value << std::endl;
  };

  ~testy()
  {
    std::cout << "testy destructor firing " << value << std::endl;
  }

  int value;
};
