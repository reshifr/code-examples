#include <cmath>
#include <crypto++/hex.h>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <mutex>
#include <stack>
#include <stdexcept>
#include <thread>
#include <vector>
#include <functional>
#include <future>
#include <array>
#include <algorithm>
#include <type_traits>

#include <cryptopp/cryptlib.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

namespace bru {

/**
 * Types
 */
using byte = unsigned char;

/**
 * Exceptions
 */
class elm_exception : public std::exception {
public:
  static constexpr auto MESSAGE =
    "Error: Unable to select element as it is empty.";
  const char* what() const noexcept override { return MESSAGE; }
};

/**
 * Functions
 */

/**
 * \brief General function to `m\choose{k}`.
 * \param m Members
 * \param k k-combination
 * \param eval Function called for each combination generated
 */
template <typename Tp>
void mck(
  const std::vector<Tp>& m, std::size_t k,
  const std::function<bool (const std::vector<Tp>& c)>& eval) noexcept {
  static_assert(
    std::is_fundamental<Tp>::value,
    "Parameter `Tp` must be of primitive type.");
  std::vector<Tp> c(k);
  std::vector<std::size_t> hk;
  hk.reserve(k);
  for(;;) {
    if( hk.size()!=k ) {
      hk.push_back(0);
      continue;
    }
    size_t ci = 0;
    for(auto i : hk)
      c[ci++] = m[i];
    if( !eval(c) )
      break;
    while( !hk.empty() && ++hk.back()==m.size() )
      hk.pop_back();
    if( hk.empty() )
      break;
  }
}

/**
 * Hash
 */
template <typename Tp>
class md5 {
  static_assert(
    std::is_fundamental<Tp>::value,
    "Parameter `Tp` must be of primitive type.");
  CryptoPP::Weak::MD5 m_hash;

public:
  static constexpr auto MD5_LENGTH =
    CryptoPP::Weak::MD5::DIGESTSIZE;
  using block = std::array<byte, MD5_LENGTH>;
  block operator()(const std::vector<Tp>& blk) noexcept {
    block hv;
    m_hash.CalculateDigest(hv.data(),
      reinterpret_cast<const byte*>(blk.data()),
      blk.size()*sizeof(typename std::vector<Tp>::value_type));
    return hv;
  }

  static block toblock(const std::string& hex) noexcept(false) {
    constexpr auto md5s_length = 2*MD5_LENGTH;
    if( hex.size()!=md5s_length )
      throw std::invalid_argument(
        "Error: Invalid MD5 string format for the given string.");
    block hv;
    CryptoPP::HexDecoder dec;
    dec.Put(reinterpret_cast<const byte*>(hex.data()), md5s_length);
    dec.MessageEnd();
    dec.Get(hv.data(), hv.size());
    return hv;
  }
};

/**
 * Main classes
 */
template <typename Tp, class H=md5<Tp>>
class par {
  std::size_t m_prk;
  const std::vector<Tp> m_m;

public:
  using hash = H;
  using hash_block = typename H::block;
  par(const std::vector<Tp>& m) noexcept(false): m_m(m) {
    if( m.empty() )
      throw elm_exception();
    m_prk = 1;
    auto hwth = std::thread::hardware_concurrency();
    while( std::pow(m_m.size(), m_prk)<hwth )
      ++m_prk;
  }

  std::vector<Tp> crack(const hash_block& hv) {
    std::vector<Tp> rc;
    std::size_t th = std::pow(m_m.size(), m_prk);
    rc = linear(th, hv);
    if( !rc.empty() )
      return rc;
    std::size_t subk=1;
    while( subk<std::numeric_limits<std::size_t>::max() ) {
      std::cout<<"Iteration: "<<subk<<std::endl;
      std::vector<std::vector<Tp>> pr;
      mck<Tp>(m_m, m_prk, [&pr](const auto& prc) {
        pr.push_back(prc);
        return true;
      });
      rc = recursive(th, subk++, hv, pr);
      if( !rc.empty() )
        return rc;
    }
    return rc;
  }

private:
  std::vector<Tp> linear(std::size_t th, const hash_block& hv) {
    std::mutex mtx;
    bool again = true;
    std::vector<Tp> rc;
    std::vector<std::future<void>> procs(th);
    // Parallel processing
    for(std::size_t i=0; i<th; ++i) {
      procs[i] = std::async(
        std::launch::async, mck<Tp>, m_m, m_prk,
        [hv, &mtx, &rc, &again](const auto& c) {
          H hash;
          if( hash(c)==hv ) {
            std::lock_guard<std::mutex> lock(mtx);
            rc = c;
            again = false;
          }
          return again;
        }
      );
    }
    return rc;
  }

  std::vector<Tp> recursive(
    std::size_t th,
    std::size_t subk,
    const hash_block& hv,
    const std::vector<std::vector<Tp>>& pr) {
    std::mutex mtx;
    bool again = true;
    std::vector<Tp> rc;
    // Parallel processing
    std::vector<std::future<void>> procs(th);
    for(std::size_t i=0; i<th; ++i) {
      auto prc = pr[i];
      procs[i] = std::async(
        std::launch::async, mck<Tp>, m_m, subk,
        [hv, &mtx, &rc, &again, prc](const auto& subc) {
          H hash;
          std::vector<Tp> c = prc;
          c.insert(c.end(), subc.begin(), subc.end());
          if( hash(c)==hv ) {
            std::lock_guard<std::mutex> lock(mtx);
            rc = c;
            again = false;
          }
          return again;
        }
      );
    }
    return rc;
  }
};

}

std::array<bru::byte, 16> strto16block(const char hex[33]) {
  std::array<bru::byte, 16> bytes;
  CryptoPP::HexDecoder decoder;
  decoder.Put(reinterpret_cast<const bru::byte*>(hex), 32);
  decoder.MessageEnd();
  decoder.Get(bytes.data(), bytes.size());
  return bytes;
}

int main(void) {
  std::string sm = "abcdefghijklmnopqrstuvwxyz";
  std::vector<char> m(sm.begin(), sm.end());
  bru::par<char> p(m);

  bru::par<char, bru::md5<char>>::hash_block hv =
    bru::md5<char>::toblock("c6995ce71bd2954357fd403958069b7a");

  auto c = p.crack(hv);
  auto cs = std::string(c.begin(), c.end());
  std::cout<<"Result: "<<cs<<std::endl;
  
  return 0;
}
