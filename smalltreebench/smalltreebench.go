package main

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/emirpasic/gods/v2/trees/avltree"
	"github.com/emirpasic/gods/v2/trees/btree"
	"github.com/emirpasic/gods/v2/trees/redblacktree"
)

type Order int8

const Len = 32

func main() {
	const n = 16
	const r = 127
	dummy := map[Order][Len]byte{}
	seed := rand.NewSource(time.Now().UnixNano())
	rng := rand.New(seed)
	for i := 0; i < n; i++ {
		k := Order(rng.Intn(r))
		v := [Len]byte{}
		rng.Read(v[:])
		dummy[k] = v
	}
	e := [Len]byte{}

	// insert
	bt := btree.New[Order, [Len]byte](8)
	start := time.Now()
	for k, v := range dummy {
		bt.Put(k, v)
	}
	end := time.Now()
	fmt.Printf("bt=%v\n", end.Sub(start))

	avlt := avltree.New[Order, [Len]byte]()
	start = time.Now()
	for k, v := range dummy {
		avlt.Put(k, v)
	}
	end = time.Now()
	fmt.Printf("avlt=%v\n", end.Sub(start))

	rbt := redblacktree.New[Order, [Len]byte]()
	start = time.Now()
	for k, v := range dummy {
		rbt.Put(k, v)
	}
	end = time.Now()
	fmt.Printf("rbt=%v\n", end.Sub(start))

	m := make(map[Order][Len]byte)
	start = time.Now()
	for k, v := range dummy {
		m[k] = v
	}
	end = time.Now()
	fmt.Printf("m=%v\n", end.Sub(start))

	fmt.Println()

	// search
	start = time.Now()
	for k := range dummy {
		e, _ = bt.Get(k)
	}
	end = time.Now()
	fmt.Printf("bt=%v\n", end.Sub(start))

	start = time.Now()
	for k := range dummy {
		e, _ = avlt.Get(k)
	}
	end = time.Now()
	fmt.Printf("avlt=%v\n", end.Sub(start))

	start = time.Now()
	for k := range dummy {
		e, _ = rbt.Get(k)
	}
	end = time.Now()
	fmt.Printf("rbt=%v\n", end.Sub(start))

	start = time.Now()
	for k := range dummy {
		e = m[k]
	}
	end = time.Now()
	fmt.Printf("m=%v\n", end.Sub(start))
	_ = e
}
