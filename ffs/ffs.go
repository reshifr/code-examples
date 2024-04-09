package main

import "fmt"

func CTZ64(n uint64) int {
	p := 0
	if (n & 0x00000000ffffffff) == 0 {
		p += 32
		n >>= 32
	}
	if (n & 0x000000000000ffff) == 0 {
		p += 16
		n >>= 16
	}
	if (n & 0x00000000000000ff) == 0 {
		p += 8
		n >>= 8
	}
	if (n & 0x000000000000000f) == 0 {
		p += 4
		n >>= 4
	}
	if (n & 0x0000000000000003) == 0 {
		p += 2
		n >>= 2
	}
	if (n & 0x0000000000000001) == 0 {
		p += 1
	}
	return p
}

func CLZ64(n uint64) int {
	p := 0
	if (n & 0xffffffff00000000) == 0 {
		p += 32
		n <<= 32
	}
	if (n & 0xffff000000000000) == 0 {
		p += 16
		n <<= 16
	}
	if (n & 0xff00000000000000) == 0 {
		p += 8
		n <<= 8
	}
	if (n & 0xf000000000000000) == 0 {
		p += 4
		n <<= 4
	}
	if (n & 0xc000000000000000) == 0 {
		p += 2
		n <<= 2
	}
	if (n & 0x8000000000000000) == 0 {
		p += 1
	}
	return p
}

func main() {
	n := uint64(0b0000_1000)
	fmt.Printf("CTZ64(n)=%v\n", CTZ64(n))
	fmt.Printf("CLZ64(n)=%v\n", CLZ64(n))
}
