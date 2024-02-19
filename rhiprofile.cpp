#include <rhi.h>
#include <xxh3.h>
#include <glib.h>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>
#include <cryptopp/rdrand.h>

using namespace std;

size_t hrhi(const void* k) {
  auto kl = strlen(reinterpret_cast<const char*>(k));
  return XXH32(k, kl, 77341);
}

bool eqrhi(const void* ki, const void* kj) {
  return strcmp(reinterpret_cast<const char*>(ki),
    reinterpret_cast<const char*>(kj))==0;
}

guint hghash(gconstpointer k) {
  auto kl = strlen(reinterpret_cast<const char*>(k));
  return XXH32(k, kl, 77341);
}

gboolean eqghash(gconstpointer ki, gconstpointer kj) {
  return strcmp(reinterpret_cast<const char*>(ki),
    reinterpret_cast<const char*>(kj))==0;
}

constexpr char CHS[] =
  "hello world";
  // "abcdefghijklmnopqrstuvwxyz"
  // "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  // "0123456789#%&+,-./=?@\\_~";
constexpr auto CHSZ = sizeof(CHS)-1;

vector<char*> rand_ks(size_t n, uint32_t min_kl, uint32_t max_kl) {
  vector<char*> ks;
  if( min_kl>max_kl )
    return ks;
  CryptoPP::RDRAND rng;
  ks.resize(n);
  for(auto& lk : ks) {
    uint32_t kl;
    rng.GenerateBlock(reinterpret_cast<CryptoPP::byte*>(&kl), sizeof(kl));
    kl = min_kl+kl%(max_kl-min_kl+1);
    auto k = new char[kl+1]();
    rng.GenerateBlock(reinterpret_cast<CryptoPP::byte*>(k), kl);
    for_each_n(k, kl, [](auto& ek) { ek = CHS[ek%CHSZ]; });
    lk = k;
  }
  return ks;
}

void del_ks(vector<char*>& ks) {
  for(auto k : ks)
    delete[] k;
}

void insert_test(void) {
  auto mrhi = rhim_init(hrhi, eqrhi, RHI_EXTEND);
  auto mghash = g_hash_table_new(hghash, eqghash);
  auto ks = rand_ks(30'000'000, 8, 16);

  auto t0 = chrono::high_resolution_clock::now();
  for(auto k : ks)
    rhim_insert(mrhi, k, (void*)true);
  auto t1 = chrono::high_resolution_clock::now();
  auto t = chrono::duration_cast<chrono::milliseconds>(t1-t0).count();
  cout<<"librhi insert = "<<t<<" ms"<<endl;
  cout<<"librhi size = "<<rhim_count(mrhi)<<endl;
  rhim_free(mrhi);

  t0 = chrono::high_resolution_clock::now();
  for(auto k : ks)
    g_hash_table_insert(mghash, k, (void*)true);
  t1 = chrono::high_resolution_clock::now();
  t = chrono::duration_cast<chrono::milliseconds>(t1-t0).count();
  cout<<"ghash insert = "<<t<<" ms"<<endl;
  cout<<"ghash size = "<<g_hash_table_size(mghash)<<endl;
  g_hash_table_destroy(mghash);

  del_ks(ks);
}

void search_test(void) {
  auto mrhi = rhim_init(hrhi, eqrhi, RHI_EXTEND);
  auto mghash = g_hash_table_new(hghash, eqghash);
  auto ks = rand_ks(30'000'000, 8, 16);

  for(auto k : ks)
    rhim_insert(mrhi, k, (void*)true);
  auto t0 = chrono::high_resolution_clock::now();
  int r = 1;
  for(auto k : ks)
    r &= (int)rhim_contains(mrhi, k);
  auto t1 = chrono::high_resolution_clock::now();
  auto t = chrono::duration_cast<chrono::milliseconds>(t1-t0).count();
  cout<<"librhi search = "<<t<<" ms"<<endl;
  cout<<"librhi size = "<<rhim_count(mrhi)<<endl;
  cout<<"librhi r = "<<r<<endl;
  rhim_free(mrhi);

  for(auto k : ks)
    g_hash_table_insert(mghash, k, (void*)true);
  t0 = chrono::high_resolution_clock::now();
  r = 1;
  for(auto k : ks)
    r &= (int)g_hash_table_contains(mghash, k);
  t1 = chrono::high_resolution_clock::now();
  t = chrono::duration_cast<chrono::milliseconds>(t1-t0).count();
  cout<<"ghash insert = "<<t<<" ms"<<endl;
  cout<<"ghash size = "<<g_hash_table_size(mghash)<<endl;
  cout<<"ghash r = "<<r<<endl;
  g_hash_table_destroy(mghash);

  del_ks(ks);
}

void delete_test(void) {
  auto mrhi = rhim_init(hrhi, eqrhi, RHI_EXTEND);
  auto mghash = g_hash_table_new(hghash, eqghash);
  auto ks = rand_ks(30'000'000, 8, 16);

  for(auto k : ks)
    rhim_insert(mrhi, k, (void*)true);
  auto t0 = chrono::high_resolution_clock::now();
  for(auto k : ks)
    rhim_delete(mrhi, k);
  auto t1 = chrono::high_resolution_clock::now();
  auto t = chrono::duration_cast<chrono::milliseconds>(t1-t0).count();
  cout<<"librhi delete = "<<t<<" ms"<<endl;
  cout<<"librhi size = "<<rhim_count(mrhi)<<endl;
  rhim_free(mrhi);

  for(auto k : ks)
    g_hash_table_insert(mghash, k, (void*)true);
  t0 = chrono::high_resolution_clock::now();
  for(auto k : ks)
    g_hash_table_remove(mghash, k);
  t1 = chrono::high_resolution_clock::now();
  t = chrono::duration_cast<chrono::milliseconds>(t1-t0).count();
  cout<<"ghash delete = "<<t<<" ms"<<endl;
  cout<<"ghash size = "<<g_hash_table_size(mghash)<<endl;
  g_hash_table_destroy(mghash);

  del_ks(ks);
}

int main(void) {
  insert_test();
  cout<<endl;

  search_test();
  cout<<endl;

  delete_test();
  return 0;
}
