#pragma once


#include <array>
#include <cstring>
#include <iostream>
#include <iomanip>

// represents a byte addressable chunk of memory
template <int NumBytes>
class __attribute__((__packed__)) block {
public:
  static constexpr size_t m_bytes = NumBytes;

  using value_type              = uint8_t;
  using reference               = value_type&;
  using const_reference         = const value_type&;
  using pointer                 = value_type*;
  using const_pointer           = const pointer;
  using container_type          = std::array<value_type, NumBytes>;
  using iterator                = typename container_type::iterator;
  using const_iterator          = typename container_type::const_iterator;
  using reverse_iterator        = typename container_type::reverse_iterator;
  using const_reverse_iterator  = typename container_type::const_reverse_iterator;
  using size_type               = size_t;
  using difference_type         = typename container_type::difference_type;

  void clear()
  {
    memset(this, 0, sizeof(*this));
  }

  block()
  {
    clear();
  }

  explicit block(const value_type& val)
  {
    fill(val);
  }

  // copy assignment
  block& operator=(const block& rhs)
  {
    memcpy(this, rhs.data(), rhs.size());
    return *this;
  }

  // copy construction
  block(const block& source)
  {
    *this = source;
  }

  // move assignment
  block& operator=(block&& rhs) noexcept
  {
    if (this != &rhs) {
      memcpy(this, rhs.data(), rhs.size());
      rhs.clear();
    }
    return *this;
  }

  // move construction
  block(block&& source) noexcept
  {
    *this = std::move(source);
  }

  // forward iterators to internal storage
  iterator begin() { return m_storage.begin(); }
  iterator end() { return m_storage.end(); }

  reverse_iterator rbegin() { return m_storage.rbegin(); }
  reverse_iterator rend() { return m_storage.rend(); }

  const_iterator cbegin() const { return m_storage.cbegin(); }
  const_iterator cend() const { return m_storage.cend(); }

  const_reverse_iterator crbegin() const { return m_storage.crbegin(); }
  const_reverse_iterator crend() const { return m_storage.crend(); }

  // block size
  size_t size()
  {
    return NumBytes;
  }

  size_t size() const
  {
    return NumBytes;
  }

  // all we zero'd out?
  bool zero() const
  {
    return checkzero(data(), NumBytes);
  }

  // return a byte addresable pointer to the base of this storage
  unsigned char* data()
  {
    return reinterpret_cast<unsigned char*>(this);
  }

  const unsigned char* data() const
  {
    // the catch 22 is you can't call reinterpret cast in const context
    // and const_cast cannot fundementally change the type,
    // so you must resort to C style casts first and then constify it.
    return const_cast<const unsigned char*>((unsigned char*)this);
  }

  void fill(const value_type& val)
  {
    memset(this, val, NumBytes);
  }

  reference operator[](size_type n)
  {
    return m_storage[n];
  }

  const_reference operator[](size_type n) const
  {
    return m_storage[n];
  }

  reference front() { return m_storage.front(); }
  const_reference front() const { return m_storage.front(); }

  reference back() { return m_storage.back(); }
  const_reference back() const { return m_storage.back(); }

  // return a string representation of the memory
  std::string str() const
  {
    std::string buf(NumBytes*2, '0');
    std::stringstream ss(buf);

    for (auto ix = cbegin(); ix != cend(); ++ix) {
      ss << std::hex << std::setfill('0') << std::setw(2) <<  (static_cast<unsigned int>(*ix) & 0x000000FF);
    }

    return ss.str();
  }

  friend std::ostream& operator<<(std::ostream& os, const block& a)
  {
    os << a.str();
    return os;
  }

  // hexdump like formatting
  std::string hexdump() const
  {
    std::stringstream os;

    int skip = 0;
    for (const_iterator ix = cbegin(); ix != cend(); ++ix) {
      os << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(*ix) & 0x000000FF) << " ";
      ++skip;

      if (skip % 8 == 0) {
        os << "  ";
      }
      if (skip % 16 == 0) {
        os << "\n";
      }
    }
    return os.str();
  }

  friend bool operator==(const block& lhs, const block& rhs)
  {
    return memcmp(&lhs, &rhs, m_bytes) == 0;
  }

  friend bool operator!=(const block& lhs, const block& rhs)
  {
    return memcmp(&lhs, &rhs, m_bytes) != 0;
  }

  friend bool operator<(const block& lhs, const block& rhs)
  {
    return memcmp(&lhs, &rhs, lhs.size()) < 0;
  }

private:
  bool checkzero(const unsigned char* string, int length) const
  {
    int is_zero;
    __asm__(
        "cld\n"
        "xorb %%al, %%al\n"
        "repz scasb\n"
        : "=c"(is_zero)
        : "c"(length), "D"(string)
        : "eax", "cc");
    return !is_zero;
  }

  // allocated just like a C array except iterators work
  std::array<value_type, NumBytes> m_storage;
};
