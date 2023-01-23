
#include <bits/stdc++.h>

using namespace std;

template <class... Ts>
struct seq {};

template <class T, class... Ts>
struct seq<T, Ts...> : seq<Ts...> {
  seq(T elm, Ts... elms) : seq<Ts...>(elms...), elm(elm) {}
  T elm;
};

template <int I, class T>
struct seq_hold;

template <class T, class... Ts>
struct seq_hold<0, seq<T, Ts...>> {
  using type = T;
};

template <int I, class T, class... Ts>
struct seq_hold<I, seq<T, Ts...>> {
  using type = typename seq_hold<I-1, seq<Ts...>>::type;
};

int main(void) {
  seq<int, string, double> line(10, "10", 10.0);
  return 0;
}
