#include <bits/stdc++.h>
#include <immintrin.h>

using namespace std;

template <class V>
class blki {
  public:
    using value_type = typename V::value_type;
    using reference = typename V::reference;
    using const_reference = typename V::const_reference;
    using pointer = typename V::pointer;
    using const_pointer = typename V::const_pointer;
    using size_type = typename V::size_type;
    using difference_type = typename V::difference_type;
    using iterator_category = std::random_access_iterator_tag;

  protected:
    difference_type m_pos;

  public:
    blki(void) noexcept : m_pos{} {}
    blki(difference_type off) noexcept : m_pos{off} {}
    bool operator==(const blki& it) const noexcept { return m_pos==it.m_pos; }
    bool operator!=(const blki& it) const noexcept { return m_pos!=it.m_pos; }
    bool operator<(const blki& it) const noexcept { return m_pos<it.m_pos; }
    bool operator<=(const blki& it) const noexcept { return m_pos<=it.m_pos; }
    bool operator>(const blki& it) const noexcept { return m_pos>it.m_pos; }
    bool operator>=(const blki& it) const noexcept { return m_pos>=it.m_pos; }
};

template <class V>
class fblki : public blki<V> {
  private:
    using blki<V>::m_pos;

  public:
    using value_type = typename blki<V>::value_type;
    using reference = typename blki<V>::reference;
    using pointer = typename blki<V>::pointer;
    using size_type = typename blki<V>::size_type;
    using difference_type = typename blki<V>::difference_type;
    using iterator_category = typename blki<V>::iterator_category;

  private:
    V* m_vector;

  public:
    fblki(void) noexcept = default;
    fblki(V* vector) noexcept : m_vector{vector} {}
    fblki(V* vector, difference_type off) noexcept :
      blki<V>{off}, m_vector{vector} {}

    reference operator*(void) const noexcept
      { return (*m_vector)[m_pos]; }
    pointer operator->(void) const noexcept
      { return &(*m_vector)[m_pos]; }
    reference operator[](difference_type off) const noexcept
      { return (*m_vector)[off]; }

    fblki& operator++(void) noexcept { ++m_pos; return *this; }
    fblki operator++(int) noexcept { return fblki(m_vector, m_pos++); }
    fblki& operator--(void) noexcept { --m_pos; return *this; }
    fblki operator--(int) noexcept { return fblki(m_vector, m_pos--); }

    fblki& operator+=(difference_type off) noexcept
      { m_pos += off; return *this; }
    fblki operator+(difference_type off) const noexcept
      { return fblki(m_vector, m_pos+off); }
    friend fblki operator+(difference_type off, const fblki& it) noexcept
      { return fblki(it.m_vector, off+it.m_pos); }
    fblki& operator-=(difference_type off) noexcept
      { m_pos -= off; return *this; }
    fblki operator-(difference_type off) const noexcept
      { return fblki(m_vector, m_pos-off); }
    difference_type operator-(const fblki& it) const noexcept
      { return m_pos-it.m_pos; }
};

template<std::uint8_t Exp, class Tp>
class fblk {
  public:
    using value_type = Tp;
    using reference = Tp&;
    using const_reference = const Tp&;
    using pointer = Tp*;
    using const_pointer = const Tp*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = fblki<fblk<Exp, Tp>>;

    static constexpr size_type NUM_ELM_BLK = static_cast<size_type>(1)<<Exp;
    static constexpr size_type MASK_BLK = NUM_ELM_BLK-static_cast<size_type>(1);
    static constexpr size_type ELM_SIZE = sizeof(Tp);
    static constexpr size_type BLK_SIZE = NUM_ELM_BLK*ELM_SIZE;

  private:
    Tp* m_blk;
    FILE* m_fp;
    size_type m_iblk;
    size_type m_n;
    size_type m_nallign;
    size_type m_nblk_allign;

    // Optimize
    size_type m_lfu;
    size_type m_i;
    Tp m_elm;

  public:
    fblk(string f, size_type n) {
      m_blk = new Tp[NUM_ELM_BLK]();
      m_fp = fopen64(f.c_str(), "wrb+");
      fseeko64(m_fp, (n-1)*ELM_SIZE, SEEK_SET);
      fputc('\0', m_fp);
      m_iblk = 0;
      m_nblk_allign = (n>>Exp)+((n&MASK_BLK)!=0);
      m_nallign = m_nblk_allign<<Exp;
      m_n = n;
      size_type zblk = m_iblk==m_nblk_allign-1 &&
        (m_n&MASK_BLK)!=0 ? (m_n&MASK_BLK)*ELM_SIZE : BLK_SIZE;
      fseeko64(m_fp, m_iblk*BLK_SIZE, SEEK_SET);
      fread(m_blk, zblk, 0, m_fp);

      // Optimize
      m_lfu = 0;
      m_i = 0;
      m_elm = m_blk[0];
    }

    reference operator[](size_type i) {
      size_type iblk = i>>Exp;
      if( iblk==m_iblk ) {
        size_type ielm = i&MASK_BLK;
        return m_blk[ielm];
      }
      if( m_lfu>=1000 ) {
        m_lfu = 0;
        size_type ielm = i&MASK_BLK;
        size_type read_zblk = iblk==m_nblk_allign-1 &&
          (m_n&MASK_BLK)!=0 ? (m_n&MASK_BLK)*ELM_SIZE : BLK_SIZE;
        size_type write_zblk = m_iblk==m_nblk_allign-1 &&
          (m_n&MASK_BLK)!=0 ? (m_n&MASK_BLK)*ELM_SIZE : BLK_SIZE;
        fseeko64(m_fp, m_iblk*BLK_SIZE, SEEK_SET);
        fwrite(m_blk, write_zblk, 1, m_fp);
        fseeko64(m_fp, iblk*BLK_SIZE, SEEK_SET);
        fread(m_blk, read_zblk, 1, m_fp);
        if( iblk==(m_i>>Exp) ) {
          size_type write_ielm = m_i&MASK_BLK;
          m_blk[write_ielm] = m_elm;
        }
        m_iblk = iblk;
        return m_blk[ielm];
      }
      if( i==m_i ) {
        return m_elm;
      }
      ++m_lfu;
      if( m_iblk==(m_i>>Exp) ) {
        size_type write_ielm = m_i&MASK_BLK;
        m_blk[write_ielm];
      } else {
        fseeko64(m_fp, m_i*ELM_SIZE, SEEK_SET);
        fwrite(&m_elm, ELM_SIZE, 1, m_fp);
      }
      fseeko64(m_fp, i*ELM_SIZE, SEEK_SET);
      fread(&m_elm, ELM_SIZE, 1, m_fp);
      m_i = i;
      return m_elm;
    }

    // reference operator[](size_type i) {
    //   size_type iblk = i>>Exp;
    //   size_type ielm = i&MASK_BLK;
    //   if( iblk==m_iblk ) {
    //     return m_blk[ielm];
    //   }
    //   size_type read_zblk = iblk==m_nblk_allign-1 &&
    //     (m_n&MASK_BLK)!=0 ? (m_n&MASK_BLK)*ELM_SIZE : BLK_SIZE;
    //   size_type write_zblk = m_iblk==m_nblk_allign-1 &&
    //     (m_n&MASK_BLK)!=0 ? (m_n&MASK_BLK)*ELM_SIZE : BLK_SIZE;
    //   fseeko64(m_fp, m_iblk*BLK_SIZE, SEEK_SET);
    //   fwrite(m_blk, write_zblk, 1, m_fp);
    //   fseeko64(m_fp, iblk*BLK_SIZE, SEEK_SET);
    //   fread(m_blk, read_zblk, 1, m_fp);
    //   m_iblk = iblk;
    //   return m_blk[ielm];
    // }

    size_t size(void) { return m_n; }
    iterator begin(void) { return iterator(this); }
    iterator end(void) { return iterator(this, size()); }

    ~fblk(void) {
      size_t zblk = m_iblk==m_nblk_allign-1 &&
        (m_n&MASK_BLK)!=0 ? (m_n&MASK_BLK)*ELM_SIZE : BLK_SIZE;
      fseeko64(m_fp, m_iblk*BLK_SIZE, SEEK_SET);
      fwrite(m_blk, zblk, 1, m_fp);

      // Optimize
      if( m_iblk!=(m_i>>Exp) ) {
        fseeko64(m_fp, m_i*ELM_SIZE, SEEK_SET);
        fwrite(&m_elm, ELM_SIZE, 1, m_fp);
      }

      delete[] m_blk;
      fclose(m_fp);
    }
};

unsigned long long rdrand() {
  unsigned long long n;
  do; while( !_rdrand64_step(&n) );
  return n;
}

int main(void) {
  random_device randdev;
  mt19937_64 rng(randdev());
  uniform_int_distribution<int> pdrand(0, 9);

  {
    fblk<19, char> m("block.bin", 1'000'000);
    for_each(m.begin(), m.end(), [&](auto& elm) { elm = 'A'+pdrand(rng); });
    sort(m.begin(), m.end());
  }
  remove("block.bin");
  return 0;
}
