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

#pragma once

#include <memory>
#include <cstddef>


template < class T >
class skeleton_allocator
{
public:
  using value_type = T;

  // not actually a trait.. but std::allocator has it
  using allocator = skeleton_allocator< T >;

  //     using pointer       = value_type*;

	// XXX we're using pointer_traits AND allocator_traits when we implement an allocator
  //     this is not clear when examining the commonly found examples and even talks

  //     using const_pointer = typename std::pointer_traits<pointer>::template
  //                                                     rebind<value_type const>;
  //     using void_pointer       = typename std::pointer_traits<pointer>::template
  //                                                           rebind<void>;
  //     using const_void_pointer = typename std::pointer_traits<pointer>::template
  //                                                           rebind<const void>;

  //     using difference_type = typename std::pointer_traits<pointer>::difference_type;
  //     using size_type       = std::make_unsigned_t<difference_type>;

  //     template <class U> struct rebind {typedef allocator<U> other;};

  skeleton_allocator() noexcept
  {
  } // not required, unless used

  template < class U >
  skeleton_allocator( skeleton_allocator< U > const& ) noexcept
  {
  }

  value_type* // Use pointer if pointer is not a value_type*
      allocate( std::size_t n )
  {
    return static_cast< value_type* >(::operator new( n * sizeof( value_type ) ) );
  }

  void deallocate( value_type* p,
                   std::size_t ) noexcept // Use pointer if pointer is not a value_type*
  {
    ::operator delete( p );
  }

//     using propagate_on_container_copy_assignment = std::false_type;
//     using propagate_on_container_move_assignment = std::false_type;
//     using propagate_on_container_swap            = std::false_type;

};

template < class T, class U >
bool operator==( skeleton_allocator< T > const&, skeleton_allocator< U > const& ) noexcept
{
  return true;
}

template < class T, class U >
bool operator!=( skeleton_allocator< T > const& x, skeleton_allocator< U > const& y ) noexcept
{
  return !( x == y );
}
