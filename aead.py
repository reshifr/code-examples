from chacha20poly1305 import ChaCha20Poly1305

# 1
key = bytearray.fromhex(
    "00b7f8ef132f263f63e9c5b61549b7564b15b9e50eb793019c888d11f4231d00"
)
add = bytearray.fromhex("00401aa5699c9102659eb2238c8d3348")
nonce = bytearray.fromhex("0102030400000000000003e8")
plaintext = b"jcATNy6yermUfxbU0S@rzc+05p~2W+AD"

aead = ChaCha20Poly1305(key)
ciphertext = aead.encrypt(nonce, plaintext, add)
print(ciphertext.hex())

# 2
key = bytearray.fromhex(
    "d5ba893af8e8c9a756bf7e7daf1e335131c59f72b7240ecda34da884cf822de4"
)
add = bytearray.fromhex("f1bf66c9135fd818d5a078b4e9a871c4")
nonce = bytearray.fromhex("0102030400000000000003e8")
plaintext = b"hlu/8djzREmN.y45arUs&uPn7piEnei1"

aead = ChaCha20Poly1305(key)
ciphertext = aead.encrypt(nonce, plaintext, add)
print(ciphertext.hex())
