#include <array>
#include <mutex>
#include <future>
#include <thread>
#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <exception>
#include <functional>
#include <type_traits>

// OpenSSL
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <openssl/types.h>

// Crypto++
#include <crypto++/hex.h>
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
class md5v1 {
  static_assert(
    std::is_fundamental<Tp>::value,
    "Parameter `Tp` must be of primitive type.");

public:
  static constexpr auto MD5_LENGTH = MD5_DIGEST_LENGTH;
  using block = std::array<byte, MD5_LENGTH>;

  block operator()(const std::vector<Tp>& blk) noexcept {
    block hv;
    MD5(reinterpret_cast<const byte*>(blk.data()),
      blk.size()*sizeof(typename std::vector<Tp>::value_type),
      hv.data());
    return hv;
  }

  static block to_block(const std::string& hex) noexcept(false) {
    constexpr auto md5s_length = 2*MD5_LENGTH;
    if( hex.size()!=md5s_length )
      throw std::invalid_argument(
        "Error: Invalid MD5 string format for the given string.");
    block hv;
    BIGNUM* bn = nullptr;
    BN_hex2bn(&bn, hex.c_str());
    if( bn!=nullptr && BN_num_bytes(bn)==MD5_LENGTH )
      BN_bn2bin(bn, hv.data());
    BN_clear_free(bn);
    return hv;
  }
};

template <typename Tp>
class md5v2 {
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

  static block to_block(const std::string& hex) noexcept(false) {
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
template <typename Tp, class H=md5v2<Tp>>
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
    std::size_t k = 1;
    std::vector<Tp> rc;
    while( k<=m_prk ) {
      std::cout<<"Linear "<<k<<std::endl;
      rc = linear(k, std::pow(m_m.size(), k), hv);
      if( !rc.empty() )
        return rc;
      ++k;
    }
    std::size_t subk = 1;
    std::size_t th = std::pow(m_m.size(), m_prk);
    while( subk<std::numeric_limits<std::size_t>::max() ) {
      std::cout<<"Recursive "<<subk<<std::endl;
      std::vector<std::vector<Tp>> prcs;
      mck<Tp>(m_m, m_prk, [&prcs](const auto& prc) {
        prcs.push_back(prc);
        return true;
      });
      rc = recursive(subk, th, hv, prcs);
      if( !rc.empty() )
        return rc;
      ++subk;
    }
    return rc;
  }

private:
  std::vector<Tp> linear(
    std::size_t k,
    std::size_t th,
    const hash_block& hv) {
    std::mutex mtx;
    std::vector<Tp> rc;
    std::vector<std::vector<Tp>> cs;
    std::vector<std::future<void>> procs(th);
    mck<Tp>(m_m, k, [&cs](const auto& c)->bool {
      cs.push_back(c);
      return true;
    });
    for(std::size_t i=0; i<th; ++i) {
      procs[i] = std::async(std::launch::async, [&rc, &mtx](
        hash h,
        const hash_block& hv,
        const std::vector<Tp>& c)->void {
        if( h(c)==hv ) {
          std::lock_guard<std::mutex> lock(mtx);
          rc = c;
        }
      }, hash(), hv, cs[i]);
    }
    return rc;
  }

  std::vector<Tp> recursive(
    std::size_t k,
    std::size_t th,
    const hash_block& hv,
    const std::vector<std::vector<Tp>>& prcs) {
    std::mutex mtx;
    bool again = true;
    std::vector<Tp> rc;
    std::vector<std::future<void>> procs(th);
    for(std::size_t i=0; i<th; ++i) {
      procs[i] = std::async(std::launch::async, [&rc, &mtx, &again](
        hash h,
        std::size_t k,
        const hash_block& hv,
        const std::vector<Tp>& m,
        const std::vector<Tp>& prc)->void {
        mck<Tp>(m, k, [&](const auto& subc)->bool {
          std::vector<Tp> c = prc;
          c.insert(c.end(), subc.begin(), subc.end());
          if( h(c)==hv ) {
            std::lock_guard<std::mutex> lock(mtx);
            rc = c;
            again = false;
          }
          return again;
        });
      }, hash(), k, hv, m_m, prcs[i]);
    }
    return rc;
  }
};

}

int main(void) {
  std::string sm = "abcdefghijklmnopqrstuvwxyz";
  std::vector<char> m(sm.begin(), sm.end());
  bru::par<char, bru::md5v2<char>> p(m);
  bru::par<char, bru::md5v2<char>>::hash_block hv =
    bru::md5v2<char>::to_block("cc689fce639ab2aa69fe308cda19b703");

  auto c = p.crack(hv);
  auto cs = std::string(c.begin(), c.end());
  std::cout<<"Result: "<<cs<<std::endl;

  return 0;
}
