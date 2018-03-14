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

#pragma once

#include <iostream>
#include <memory>
#include <cstddef>

template < class T >
class counting_allocator
{
  struct context
  {
    size_t count{0};
    void increment( size_t n )
    {
      count += n;
    }
    void decrement( size_t n )
    {
      count -= n;
    }
    size_t show() const
    {
      return count;
    }
  };

public:
  using value_type = T;

  // not actually a trait.. but std::allocator has it
  using allocator = counting_allocator< T >;

  // everything else is provided for by the container accessing this
  // through the allocater_traits interface

  //  using propagate_on_container_copy_assignment = std::true_type;
  //  using propagate_on_container_move_assignment = std::true_type;
  //  using propagate_on_container_swap            = std::true_type;

  std::shared_ptr< context > context_ptr;

  counting_allocator() noexcept
  {
    context_ptr = std::make_shared< context >();
    std::cout << "(" << this << " --ctor)\n";
  }

  ~counting_allocator()
  {
    std::cout << "(" << this << " --dtor, with bytes outstanding: " << count() << ")\n";
  }


  // copy construct
  counting_allocator( const counting_allocator& other )
      : counting_allocator()
  {
    context_ptr = other.context_ptr;
    std::cout << "(" << this << " --copy_ctor)\n";
  }

  // copy assign
  counting_allocator& operator=( const counting_allocator& other )
  {
    std::cout << "(" << this << " --copy_assignment)\n";
    if ( this != &other )
    {
      context_ptr = other.context_ptr;
    }
    return *this;
  }


#if 0
  // move assignment
  counting_allocator& operator=( counting_allocator&& other ) //noexcept
  {
    std::cout << "(--move assign )\n";
    if ( this != &other )
    {
      // copy the context ptr
      context_ptr = other.context_ptr;

      // let scope handle reference counting of shared_ptr;
    }
    return *this;
  }

  // move construction
  counting_allocator(counting_allocator&& source) //noexcept
  {
    std::cout << "(--move construct) \n";
    *this = std::move(source);
  }
#endif

  value_type* allocate( std::size_t n )
  {
    std::size_t amount = n * sizeof( value_type );

    std::cout << "("<< this <<"--allocating bytes: " << amount << ")\n";

    context_ptr->increment( amount );
    return static_cast< value_type* >(::operator new( amount ) );
  }

  void deallocate( value_type* p, std::size_t n ) noexcept
  {
    std::size_t amount = n * sizeof( value_type );

    std::cout << "("<< this <<"--deallocating bytes: " << amount << ")\n";

    context_ptr->decrement( amount );
    ::operator delete( p );
  }

  std::size_t count() const
  {
    return context_ptr->show();
  }

};

// when allocators are equal, they can deallocate another allocator's stuff
template < class T, class U >
bool operator==( counting_allocator< T > const&, counting_allocator< U > const& ) noexcept
{
  return true;
}

template < class T, class U >
bool operator!=( counting_allocator< T > const& x, counting_allocator< U > const& y ) noexcept
{
  return !( x == y );
}
