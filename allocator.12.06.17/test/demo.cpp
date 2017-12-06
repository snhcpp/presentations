// SNHCPP Introduction to std::allocator
// Author: Peter M. Petrakis <peter.petrakis@gmail.com>
// No License, do what you want

// resources:
// -- http://en.cppreference.com/w/cpp/memory/allocator
// -- http://en.cppreference.com/w/cpp/memory/allocator_traits
// -- https://stackoverflow.com/questions/872675/policy-based-design-and-best-practices-c
// -- https://stackoverflow.com/questions/14718055/what-is-the-difference-between-a-trait-and-a-policy
// -- https://stackoverflow.com/questions/19803954/template-template-parameters-and-clang
// -- https://codereview.stackexchange.com/questions/31528/a-working-stack-allocator
// -- https://stackoverflow.com/questions/826569/compelling-examples-of-custom-c-allocators#826635

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

// end demo.cpp
