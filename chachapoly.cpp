#include <cryptopp/cryptlib.h>
#include <cryptopp/chachapoly.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>

// g++ chachapoly.cpp -o /tmp/chachapoly -lcryptopp; /tmp/chachapoly
int main(int argc, char *argv[])
{
  using namespace CryptoPP;

  constexpr byte plaintext[] = {'X'};
  constexpr byte aad[] = {};
  constexpr byte key[] = {
      0x82, 0x60, 0xd8, 0x20, 0x66, 0x5d, 0xbc, 0xbc,
      0xbf, 0x7a, 0xd2, 0xf5, 0x48, 0x82, 0xef, 0x78,
      0x6d, 0x7c, 0xc8, 0xa8, 0xc9, 0x7b, 0x4e, 0x7b,
      0xa6, 0x28, 0xa1, 0x02, 0x09, 0xcc, 0x53, 0x1d};
  constexpr byte iv[] = {
      0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x03, 0xe8};

  byte ciphertext[sizeof(plaintext)];
  byte mac[16];

  ChaCha20Poly1305::Encryption enc;
  enc.SetKeyWithIV(key, sizeof(key), iv, sizeof(iv));
  enc.EncryptAndAuthenticate(
      ciphertext,
      mac, sizeof(mac),
      iv, sizeof(iv),
      aad, sizeof(aad),
      plaintext, sizeof(plaintext));

  std::cout << "Plain: ";
  StringSource(
      plaintext, sizeof(plaintext),
      true, new HexEncoder(new FileSink(std::cout), false));
  std::cout << std::endl;

  std::cout << "Cipher: ";
  StringSource(
      ciphertext, sizeof(ciphertext),
      true, new HexEncoder(new FileSink(std::cout), false));
  std::cout << std::endl;

  std::cout << "MAC: ";
  StringSource(
      mac, sizeof(mac),
      true, new HexEncoder(new FileSink(std::cout), false));
  std::cout << std::endl;
  return 0;
}