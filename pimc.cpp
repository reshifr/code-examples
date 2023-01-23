#include <bits/stdc++.h>
#include <immintrin.h>

using namespace std;

using f = float;
using d = double;
using i = int;
using ui = unsigned int;
using ll = long long;
using ull = unsigned long long;

d rdrandd() {
  ui n;
  do; while( !_rdrand32_step(&n) );
  return static_cast<d>(n)/static_cast<d>(numeric_limits<ui>::max());
}

d pimc(ull p) {
  ull m = 0;
  ull n = p;
  for(ull i=0; i<p; ++i) {
    d x = rdrandd();
    d y = rdrandd();
    d c = x*x+y*y;
    if( sqrt(c)<=1.0 )
      ++m;
  }
  return 4.0*m/n;
}


int main(void) {
  d pi = pimc(100000);
  cout<<"pi = "<<pi<<endl;
}
