#include <bits/stdc++.h>

class buku {
  char* isi;

  public:
    buku(void) {
      std::cout<<"buku dibuat"<<std::endl;
      isi = new char[6];
    }

    buku(const buku& lain) {
      std::cout<<"buku dibuat dari isi buku lain"<<std::endl;
      isi = new char[6];
      std::memcpy(isi, lain.isi, 6);
    }

    buku(buku&& lain) {
      std::cout<<"isi buku lain dipindahkan ke buku saat ini"<<std::endl;
      isi = lain.isi;
      lain.isi = nullptr;
    }

    buku& operator=(const buku& lain) {
      std::cout<<"buku dibuat dari isi buku lain, isi buku saat ini dibuang"<<std::endl;
      delete[] isi;
      isi = new char[6];
      std::memcpy(isi, lain.isi, 6);
      return *this;
    }

    buku& operator=(buku&& lain) {
      std::cout<<"isi buku lain ditukar dengan isi buku saat ini"<<std::endl;
      std::swap(isi, lain.isi);
      return *this;
    }

    ~buku(void) {
      std::cout<<"buku dihancurkan"<<std::endl;
      delete[] isi;
    }
};

int main() {
  return 0;
}
