package main

/*
#cgo CFLAGS: -Ofast -mtune=native -march=native

#include <stdio.h>
#include <stdint.h>

typedef int (*popcnt)(uint64_t);

int add_popcountll(uint64_t n) {
	n -= (n>>1)&0x5555555555555555;
	n = (n&0x3333333333333333)+((n>>2)&0x3333333333333333);
	n = (n+(n>>4))&0x0f0f0f0f0f0f0f0f;
	n += n>>8;
	n += n>>16;
	n += n>>32;
	return n&0x7f;
}

int mul_popcountll(uint64_t n) {
	n -= (n>>1)&0x5555555555555555;
	n = (n&0x3333333333333333)+((n>>2)&0x3333333333333333);
	n = (n+(n>>4))&0x0f0f0f0f0f0f0f0f;
	return (n*0x0101010101010101)>>56;
}

int64_t POPCNT64_CAdd(void* b, int64_t n) {
	int64_t w = 0;
	uint64_t* block = (uint64_t*)b;
	for(int64_t i=0; i<n; ++i) {
		w += add_popcountll(block[i]);
	}
	return w;
}

int64_t POPCNT64_CMul(void* b, int64_t n) {
	int64_t w = 0;
	uint64_t* block = (uint64_t*)b;
	for(int64_t i=0; i<n; ++i) {
		w += mul_popcountll(block[i]);
	}
	return w;
}

int64_t POPCNT64_CBuiltin(void* b, int64_t n) {
	int64_t w = 0;
	uint64_t* block = (uint64_t*)b;
	for(int64_t i=0; i<n; ++i) {
		w += __builtin_popcountll(block[i]);
	}
	return w;
}
*/
import "C"

import (
	"fmt"
	"math/bits"
	"math/rand"
	"time"
	"unsafe"
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
	fmt.Println()

	block := make([]uint64, (8*1024*1024)/8)
	seed := rand.NewSource(time.Now().UnixNano())
	rng := rand.New(seed)
	for i := 0; i < len(block); i++ {
		block[i] = rng.Uint64()
	}

	w := 7
	start := time.Now()
	for i := 0; i < len(block); i++ {
		w += POPCNT64_Add(block[i])
	}
	end := time.Now()
	fmt.Printf("add=%v\n", end.Sub(start))
	fmt.Println(w)
	fmt.Println()

	w = 7
	start = time.Now()
	for i := 0; i < len(block); i++ {
		w += POPCNT64_Mul(block[i])
	}
	end = time.Now()
	fmt.Printf("mul=%v\n", end.Sub(start))
	fmt.Println(w)
	fmt.Println()

	w = 7
	start = time.Now()
	for i := 0; i < len(block); i++ {
		w += bits.OnesCount64(block[i])
	}
	end = time.Now()
	fmt.Printf("std=%v\n", end.Sub(start))
	fmt.Println(w)
	fmt.Println()

	w = 7
	start = time.Now()
	w += int(C.POPCNT64_CAdd(unsafe.Pointer(&block[0]), C.int64_t(len(block))))
	end = time.Now()
	fmt.Printf("cadd=%v\n", end.Sub(start))
	fmt.Println(w)
	fmt.Println()

	w = 7
	start = time.Now()
	w += int(C.POPCNT64_CMul(unsafe.Pointer(&block[0]), C.int64_t(len(block))))
	end = time.Now()
	fmt.Printf("cmul=%v\n", end.Sub(start))
	fmt.Println(w)
	fmt.Println()

	w = 7
	start = time.Now()
	w += int(C.POPCNT64_CBuiltin(unsafe.Pointer(&block[0]), C.int64_t(len(block))))
	end = time.Now()
	fmt.Printf("gnu=%v\n", end.Sub(start))
	fmt.Println(w)
}
