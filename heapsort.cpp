#include <bits/stdc++.h>
#include <immintrin.h>

using namespace std;

template<class Tp>
void shiftdown(Tp* blk, size_t a, size_t b) {
  ptrdiff_t r = a;
  while( (r<<1)+1<=b ) {
    ptrdiff_t c = (r<<1)+1;
    ptrdiff_t swp = r;
    if( blk[swp]<blk[c] )
      swp = c;
    if( c+1<=b && blk[swp]<blk[c+1])
      swp = c+1;
    if( swp==r ) {
      break;
    }
    Tp tmp = blk[r];
    blk[r] = blk[swp];
    blk[swp] = tmp;
    r = swp;
  }
}

template<class Tp>
void heapify(Tp* blk, size_t n) {
  for(ptrdiff_t a=(n-2)>>1; a>=0; --a)
    shiftdown(blk, a, n-1);
}

template<class Tp>
void heapsort(Tp* blk, size_t n) {
  heapify(blk, n);
  ptrdiff_t b = n-1;
  while( b>0 ) {
    Tp tmp = blk[b];
    blk[b] = blk[0];
    blk[0] = tmp;
    shiftdown(blk, 0, --b);
  }
}

int main(void) {
  random_device randdev;
  mt19937_64 rng(randdev());
  uniform_int_distribution<int> pdrand(0, 9);

  vector<char> m(100'000'000);
  for_each(m.begin(), m.end(), [&](auto& elm) { elm = 'A'+pdrand(rng); });

  auto md = m.data();
  auto mn = m.size();
  heapsort(md, mn);
}
