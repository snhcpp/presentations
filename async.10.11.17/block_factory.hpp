#pragma once

#include <random>
#include <chrono>
#include <mutex>

template <class BlockType>
struct op_zero_fill {
  using block_type = BlockType;

  static void fill(block_type& block)
  {
    memset(block.data(), 0, block.size());
  }
};

template <class BlockType>
struct op_random_fill {
  using block_type = BlockType;

  static void fill(block_type& block)
  {
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    //unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine  rd(seed);
    std::mt19937_64 gen(rd());
    for(auto& byte: block) {
      std::uniform_int_distribution<int> dis(0, 255);
      byte = dis(gen);
    }
  }
};

template <class BlockType,
          template <class> class FillPolicy = op_zero_fill>
struct block_factory {
  using block_type       = BlockType;
  using fill_policy_type = FillPolicy<BlockType>;
  //static std::mutex m_lock;

  static block_type create()
  {
    //std::unique_lock<decltype(m_lock)> locker;
    block_type b;
    fill_policy_type::fill(b);
    return b;
  }
};
