package main

import (
	"fmt"
	"math/bits"
	"math/rand"
	"time"
)

func POPCNT64_Add(n uint64) int {
	n -= (n >> 1) & 0x5555555555555555
	n = (n & 0x3333333333333333) + ((n >> 2) & 0x3333333333333333)
	n = (n + (n >> 4)) & 0x0f0f0f0f0f0f0f0f
	n += n >> 8
	n += n >> 16
	n += n >> 32
	return int(n & 0x7f)
}

func POPCNT64_Mul(n uint64) int {
	n -= (n >> 1) & 0x5555555555555555
	n = (n & 0x3333333333333333) + ((n >> 2) & 0x3333333333333333)
	n = (n + (n >> 4)) & 0x0f0f0f0f0f0f0f0f
	return int((n * 0x0101010101010101) >> 56)
}

func main() {
	n := uint64(0b00010_0011)
	fmt.Printf("POPCNT64_Add(n)=%v\n", POPCNT64_Add(n))
	fmt.Printf("POPCNT64_Mul(n)=%v\n", POPCNT64_Mul(n))

	block := make([]uint64, (8*1024*1024)/8)
	rand.Seed(time.Now().Unix())
	for i := 0; i < len(block); i++ {
		block[i] = rand.Uint64()
	}

	w := 7
	start := time.Now()
	for i := 0; i < len(block); i++ {
		w += POPCNT64_Add(block[i])
	}
	end := time.Now()
	fmt.Printf("add=%v\n", end.Sub(start))
	fmt.Println(w)

	w = 7
	start = time.Now()
	for i := 0; i < len(block); i++ {
		w += POPCNT64_Mul(block[i])
	}
	end = time.Now()
	fmt.Printf("mul=%v\n", end.Sub(start))
	fmt.Println(w)

	w = 7
	start = time.Now()
	for i := 0; i < len(block); i++ {
		w += bits.OnesCount64(block[i])
	}
	end = time.Now()
	fmt.Printf("std=%v\n", end.Sub(start))
	fmt.Println(w)
}
