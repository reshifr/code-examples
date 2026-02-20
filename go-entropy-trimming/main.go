package main

import (
	"crypto/rand"
	"fmt"
	"math"
	"math/big"

	"golang.org/x/crypto/argon2"
)

func main() {
	base := uint8(58)
	chars := uint32(20)
	salt := make([]byte, 64)
	keyLen := uint32(math.Ceil(math.Log2(float64(base)*float64(chars)) / 8))
	X := new(big.Int)
	for {
		rand.Read(salt)
		b256 := big.NewInt(256)
		m256 := big.NewInt(int64(keyLen))
		bX := big.NewInt(int64(base))
		mX := big.NewInt(int64(chars))
		S := new(big.Int).Exp(b256, m256, nil)
		T := new(big.Int).Exp(bX, mX, nil)
		q := new(big.Int).Div(S, T)
		R := new(big.Int).Mul(q, T)
		key := argon2.Key([]byte("GILA"), salt, 3, 32*1024, 4, keyLen)
		N := new(big.Int).SetBytes(key)
		if N.Cmp(R) >= 0 {
			continue
		} else {
			X.Mod(N, T)
		}
	}
	fmt.Println(X)
}
