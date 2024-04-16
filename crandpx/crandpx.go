package main

import (
	"crypto/rand"
	"encoding/binary"
	"fmt"
	"math/bits"
)

func main() {
	const n = 1 << 32
	dup := make([]uint64, n>>6)
	v := 1000_000
	b := make([]byte, v<<2)
	rand.Read(b)

	for i := 0; i < v; i++ {
		r := binary.BigEndian.Uint32(b[i<<2:])
		dup[r>>6] |= 1 << (r & 63)
	}

	w := 0
	for i := 0; i < (n >> 6); i++ {
		w += bits.OnesCount64(dup[i])
	}
	fmt.Println(v)
	fmt.Println(w)
	fmt.Println(float64(1) - float64(w)/float64(v))
}
