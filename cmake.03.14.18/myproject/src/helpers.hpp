#include <cxxabi.h>  // needed for abi::__cxa_demangle
#include <memory>
#include <sstream>
#include <iostream>

// https://stackoverflow.com/questions/4939636/function-to-mangle-demangle-functions

std::shared_ptr< char > cppDemangle( const char* abiName )
{
  int   status;
  char* ret = abi::__cxa_demangle( abiName, 0, 0, &status );

  /* NOTE: must free() the returned char when done with it! */
  std::shared_ptr< char > retval;
  retval.reset( (char*)ret, []( char* mem ) {
    if ( mem )
      free( (void*)mem );
  } );
  return retval;
}

#define CLASS_NAME(somePointer) ((const char *) cppDemangle(typeid(*somePointer).name()).get() )

// mine :)
struct Inspect {

  template < typename Alloc >
  static std::string show(const Alloc& a)
  {
    // XXX learn SFINAE to work around custom allocators that don't have
    // "allocator" defined like std::allocator does
    using traits_t = std::allocator_traits< typename Alloc::allocator >;

    std::stringstream os;

    os << "-------------- " << CLASS_NAME(&a) << " -------------------" << "\n";
    os << "propagate_on_container_copy_assignment:"
       << traits_t::propagate_on_container_copy_assignment::value;
    os << "\n";

    os << "propagate_on_container_move_assignment:"
       << traits_t::propagate_on_container_move_assignment::value;
    os << "\n";

    os << "propagate_on_container_swap:"
       << traits_t::propagate_on_container_swap::value;
    os << "\n";

    // C++17
    //os << "is_always_equal:" << traits_t::is_always_equal::value;
    //os << "\n";
    os << "-------------- " << CLASS_NAME(&a) << " -------------------" << "\n";

    return os.str();
  }
};


template < typename Container >
Container shared_scale_container( const Container& c)
{
  Container ret(c.get_allocator());
  ret.reserve( c.size() );

  for ( const auto& v : c )
    ret.emplace_back( (v * 2) );

  return ret;
}

template < typename Container >
Container copy_scale_container( const Container& c )
{
  Container ret( c );

  for ( size_t i = 0; i < c.size(); ++i )
  {
    ret[i] = c[i] * 2;
  }

  return ret;
}

template < typename Container >
void dump_container( const Container& c )
{
  for ( const auto& v : c )
    std::cout << v << " ";
  std::cout << '\n';
}

template < typename Container >
Container scale_container( const Container& c )
{
  Container ret;
  ret.reserve( c.size() );

  for ( const auto& v : c )
    ret.emplace_back( (v * 2) );

  return ret;
}
