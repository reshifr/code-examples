package main

import (
	"fmt"
	"math/bits"
	"math/rand"
	"time"
)

func BitsTraversal(n uint64) int {
	w := 0
	for n != 0 {
		i := bits.TrailingZeros64(n)
		w += i
		n &= ^(1 << i)
	}
	return w
}

func BitsTraversal2(n uint64) int {
	w := 0
	for n != 0 {
		i := bits.TrailingZeros64(n)
		w += i
		n &= n - 1
	}
	return w
}

func main() {
	block := make([]uint64, (8*1024*1024)/8)
	seed := rand.NewSource(time.Now().UnixNano())
	rng := rand.New(seed)
	for i := 0; i < len(block); i++ {
		block[i] = rng.Uint64()
	}

	w := 7
	start := time.Now()
	for i := 0; i < len(block); i++ {
		w += BitsTraversal(block[i])
	}
	end := time.Now()
	fmt.Printf("BitsTraversal=%v\n", end.Sub(start))
	fmt.Println(w)

	w = 7
	start = time.Now()
	for i := 0; i < len(block); i++ {
		w += BitsTraversal2(block[i])
	}
	end = time.Now()
	fmt.Printf("BitsTraversal2=%v\n", end.Sub(start))
	fmt.Println(w)
}
