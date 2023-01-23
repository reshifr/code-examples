#include <bits/stdc++.h>

using namespace std;

template<class Tp>
void bogosort(vector<Tp>& vec) {
  while( !is_sorted(vec.begin(), vec.end()) )
    random_shuffle(vec.begin(), vec.end());
}

int main(void) {
  vector<int> m{1, 2, 8, 2, 3, 1, 3, 7, 2, 2, 1};
  bogosort(m);

  for_each(m.begin(), m.end(), [&](auto elm) { cout<<elm<<" "; });
  cout<<endl;
  return 0;
}
