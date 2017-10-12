// SNHCPP Introduction to Async
// Author: Peter M. Petrakis <peter.petrakis@gmail.com>
// No License, do what you want

// clang-format off
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// for native thread_id
#include <sys/types.h>
#include <sys/syscall.h>

#if 0
// XXX non of the examples currently use this so this is also
// disabled in the cmake file

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
// I tried moving this into it's own dir and it exploded with undefined refs
#include <mutex>
#include "stdcpp.h"
#include "cryptlib.h"
#include "filters.h"
#include "hex.h"
#include "sha.h"
#endif

#include "block.hpp"
#include "block_factory.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <functional>
#include <deque>
// clang-format on

using namespace std::chrono_literals;

// Myers type printer. Incomplete class def forces the compiler to barf when you define
// it but not before it shows you the type fed in.
template < typename T >
class ShowType;

// Returns a future where a a new thread is launched to execute the task asynchronously
template < typename Function, typename... Args >
auto make_async_future( Function&& func, Args&&... args )
{
  return std::async( std::launch::async, func, std::forward< Args >( args )... );
};

// Returns a future where a the task is executed on the calling thread the first time its result is
// requested (lazy evaluation)
template < typename Function, typename... Args >
auto make_deferred_future( Function&& func, Args&&... args )
{
  return std::async( std::launch::deferred, func, std::forward< Args >( args )... );
};

// scope based timer
class StopWatch
{
public:
  StopWatch()
      : mStart( std::chrono::high_resolution_clock::time_point{} )
      , mEnd( std::chrono::high_resolution_clock::time_point{} )
      , mStopBit( 0 )
  {
    start();
  }

  ~StopWatch()
  {
    // if we were stopped once we're not printing this again
    if ( mStopBit.load() == 0 )
    {
      mEnd = std::chrono::high_resolution_clock::now();
      std::chrono::duration< double, std::milli > elapsed = mEnd - mStart;
      std::cout << "(elapsed time=" << elapsed.count() << "ms)" << std::endl;
    }
  }

  void start()
  {
    mStart = std::chrono::high_resolution_clock::now();
  }

  void reset()
  {
    mStart   = std::chrono::high_resolution_clock::time_point{};
    mEnd     = std::chrono::high_resolution_clock::time_point{};
    mStopBit = 0;
  }

  // don't update the endtime, just return the elapsed time string
  std::string split() const
  {
    auto split = std::chrono::high_resolution_clock::now();
    std::chrono::duration< double, std::milli > elapsed = split - mStart;

    std::stringstream ss;
    ss << "(split time=" << elapsed.count() << "ms)";
    return ss.str();
  }

  // stop the clock and return the elapsed time string
  std::string stop()
  {
    mEnd = std::chrono::high_resolution_clock::now();
    mStopBit.store( 1 );

    std::chrono::duration< double, std::milli > elapsed = mEnd - mStart;
    std::stringstream ss;
    ss << "(split time=" << elapsed.count() << "ms)";
    return ss.str();
  }

  // always return the split time in this context
  friend std::ostream& operator<<( std::ostream& os, const StopWatch& rhs )
  {
    os << rhs.split();
    return os;
  }

private:
  std::chrono::high_resolution_clock::time_point mStart, mEnd;
  std::atomic< int >                             mStopBit;
}; // StopWatch

TEST( Example, WaitForIt )
{
  auto delay = 2s;

  auto waiter = [&]() {
    std::this_thread::sleep_for( delay );
    std::cout << "hello!!!" << std::endl;
  };

  // launch it
  {
    // std::future<void> fut = std::async( std::launch::async, waiter );

    StopWatch watch;

    std::future< void > fut = make_async_future( waiter );

    fut.get();

    std::cout << "async launch, " << watch.stop() << std::endl;
  }

  {
    StopWatch watch;

    std::future< void > fut = make_deferred_future( waiter );

    fut.get();

    std::cout << "deferred launch, " << watch.stop() << std::endl;
  }

  // old school
  {
    StopWatch watch;

    std::thread t1{ waiter };

    t1.join();

    std::cout << "old school thread launch, " << watch.stop() << std::endl;
  }


}


// example inspired by Bo Qian and packaged based thread pool
// https://www.youtube.com/watch?v=FfbZfBk-3rI
// http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/
std::deque< std::packaged_task< std::string() > > workq;
std::mutex                                        workq_lock;
std::condition_variable                           workq_cv;

TEST( Example, PackagedTask )
{

  // lambda was angry about capturing workq by reference...
  auto package_worker = [] {
    std::packaged_task< std::string() > t;

    {
      std::unique_lock< std::mutex > locker{workq_lock};
      workq_cv.wait( locker, []() { return !workq.empty(); } );
      t = std::move( workq.front() );
      workq.pop_front();
    }
    t();
  };

  std::thread t1( package_worker );
  auto        delay = 2s;

  auto return_waiter = [&]() -> std::string {
    std::this_thread::sleep_for( delay );
    return "hello!!!";
  };

  std::packaged_task< std::string() > task( return_waiter );

  // get a *handle* to our future value. Does not block
  std::future< std::string > fu = task.get_future();

  // add our trivial worker to the workq
  {
    std::lock_guard< std::mutex > locker{workq_lock};
    workq.push_back( std::move( task ) );
  }
  workq_cv.notify_one();

  // block until result is ready
  auto result = fu.get();
  EXPECT_TRUE( result == "hello!!!" );

  std::cout << "(packaged_task) we got " << result << std::endl;
  t1.join();
}


#if 0
// hash a string
std::string SHA1Hash( std::string source )
{
  CryptoPP::SHA1 sha1;

  std::string hash;

  CryptoPP::StringSource(
      source, true, new CryptoPP::HashFilter(
                        sha1, new CryptoPP::HexEncoder( new CryptoPP::StringSink( hash ) ) ) );
  std::transform(hash.begin(), hash.end(), hash.begin(), ::tolower);
  return hash;
}

// hash a buffer
std::string SHA1Hash( const unsigned char* source, size_t size )
{
  CryptoPP::SHA1 sha1;

  std::string hash;

  CryptoPP::ArraySource(
      source, size, true, new CryptoPP::HashFilter(
                        sha1, new CryptoPP::HexEncoder( new CryptoPP::StringSink( hash ) ) ) );
  std::transform(hash.begin(), hash.end(), hash.begin(), ::tolower);
  return hash;
}

template < int Size >
std::string SHA1Hash( const block< Size >& source )
{
  return SHA1Hash( source.data(), source.size() );
}

TEST( Example, Hashing )
{
  // string hash
  {
    std::string source = "hello";
    auto        digest = SHA1Hash( source );
    std::cout << digest << std::endl;

    // echo -n "hello" | sha1sum
    EXPECT_TRUE( digest == "aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d" );
  }

  // 4k zeroed buffer
  {
    std::array< unsigned char, 4096 > source;
    source.fill( 0 );

    auto digest = SHA1Hash( source.data(), source.size() );
    // dd if=/dev/zero bs=4096 count=1 | sha1sum
    EXPECT_TRUE( digest == "1ceaf73df40e531df3bfb26b4fb7cd95fb7bff1d" );
    std::cout << digest << std::endl;
  }
}
#endif

TEST( Example, FanOut )
{
  constexpr int capacity = 1024 * 64;

//  using BlockType          = block< 2048 >;
//  using RandomBlockFactory = block_factory< BlockType, op_random_fill >;
  using storage_type       = int;

  // write back our results
  std::vector< storage_type > results( capacity );
  std::mutex                  results_lock;

  std::cout << "main vector size " << results.size() << std::endl;

  auto worker
      = [&]( long long seed, const size_t segment_begin, const size_t segment_end, const size_t segment_size ) {

//          printf( "before calling pthread_create getpid: %d getpthread_self: %lu tid:%lu\n",
//                  getpid(), pthread_self(), syscall( SYS_gettid ) );

          // local storage
          std::vector< storage_type > slice( segment_size );

          (void) seed;
#if 0
          std::mt19937 mt_rand(seed);
          std::generate(slice.begin(), slice.end(), [&]() {return mt_rand();});
#endif

          // fill our local slice
          for ( unsigned k = 0; k < slice.size(); ++k )
          {
            slice[k]   = k;
            //slice[k] = SHA1Hash(RandomBlockFactory::create());
          }
          // merge back our results under lock

          std::unique_lock< decltype( results_lock ) > locker( results_lock );

          std::cout << "writeback (locked) segment start " << segment_begin << " and end "
                    << segment_end << std::endl;

          for ( const auto& value : slice )
          {
            for ( auto index = segment_begin; index < segment_end; ++index )
            {
              results[index] = value;
            }
          }

        }; // end lambda

  // for each cpu, determine which range a thread is responsible for a range
  // and launch an async operation to do the job
  // XXX zero check
  size_t cpu_max = std::thread::hardware_concurrency();

  std::cout << "running on " << cpu_max << " processors" << std::endl;

  const size_t segment_size = results.size() / cpu_max;
  std::cout << "segment size " << segment_size << std::endl;

  std::vector< std::future< void > > FanOut;
  StopWatch                          watch;

  for ( size_t cpu_index = 0; cpu_index <= ( cpu_max - 1 ); ++cpu_index )
  {
    // determine where to copy our results in the main vector;
    const size_t segment_begin = cpu_index * segment_size;
    const size_t segment_end   = ( ( cpu_index + 1 ) * segment_size ) - 1;

    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    seed += cpu_index;
    //std::cout << "seed " << seed << std::endl;

    // launch a future
    FanOut.emplace_back( make_async_future( worker, seed, segment_begin, segment_end, segment_size ) );
  }

  // block on results
  for ( auto& f : FanOut )
  {
    f.get();
  }

  std::cout << "fanout fill " << watch.stop() << std::endl;
}
