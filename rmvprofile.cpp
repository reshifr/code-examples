#define RMV_DEBUG

#include <chrono>
#include <thread>
#include <vector>
#include <cstdint>
#include <rmv.hpp>
#include <iostream>

using namespace std;

void rmv_test(void) {
  constexpr uint64_t n = 150'000'000;
  rsfr::rpmv<20, uint64_t> v;
  for(uint64_t i=0; i<n; ++i)
    v.push_back(i);
  this_thread::sleep_for(chrono::milliseconds(300));
  auto size = v.size();
  auto cap = v.capacity();
  cout<<"rmv_capacity="<<cap<<", wasting_memory="
    <<(100.0-static_cast<double>(size)/static_cast<double>(cap)*100)<<"%"<<endl;
}

void vector_test(void) {
  constexpr uint64_t n = 150'000'000;
  std::vector<uint64_t> v;
  for(uint64_t i=0; i<n; ++i)
    v.push_back(i);
  this_thread::sleep_for(chrono::milliseconds(300));
  auto size = v.size();
  auto cap = v.capacity();
  cout<<"vector_capacity="<<cap<<", wasting_memory="
    <<(100.0-static_cast<double>(size)/static_cast<double>(cap)*100)<<"%"<<endl;
}

void vector_growth_test(void) {
  constexpr uint64_t n = 67'108'864+1;
  std::vector<uint8_t> v;
  uint64_t growth = 0;
  for(uint64_t i=0; i<n; ++i) {
    v.push_back(static_cast<uint8_t>(i&0xff));
    auto cap = v.capacity();
    if( growth!=cap ) {
      growth = cap;
      cout<<"vector capacity="<<v.capacity()<<endl;
    }
  }
}

int main(void) {
  rmv_test();
  vector_test();
  return 0;
}