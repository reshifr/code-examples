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
	chars := uint32(200)
	bytes := uint32(math.Ceil(float64(chars) * math.Log2(float64(base)) / 8))
	salt := make([]byte, 64)
	rand.Read(salt)
	key := argon2.Key([]byte("GILA"), salt, 3, 32*1024, 4, bytes)

	b := big.NewInt(int64(base))
	m := big.NewInt(int64(chars))
	R := new(big.Int).SetBytes(key)
	T := new(big.Int).Exp(b, m, nil)
	q := new(big.Int).Div(R, T)
	limit := new(big.Int).Mul(q, T)

	fmt.Println(limit)
}
