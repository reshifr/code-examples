package main

import (
	"context"
	"fmt"
	"log"
	"time"

	"github.com/chromedp/chromedp"
)

func main() {
	// new browser, first tab
	ctx1, cancel := chromedp.NewContext(context.Background())
	defer cancel()

	// ensure the first tab is created
	if err := chromedp.Run(ctx1); err != nil {
		log.Fatal(err)
	}

	// same browser, first tab
	ctx2, cancel2 := context.WithTimeout(ctx1, 1*time.Minute)
	defer cancel2()

	// ensure the first tab is created
	if err := chromedp.Run(ctx2); err != nil {
		log.Fatal(err)
	}

	c1 := chromedp.FromContext(ctx1)
	c2 := chromedp.FromContext(ctx2)

	fmt.Printf("Same browser: %t\n", c1.Browser == c2.Browser)
	fmt.Printf("Same tab: %t\n", c1.Target == c2.Target)
}
