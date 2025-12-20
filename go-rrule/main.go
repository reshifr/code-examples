package main

import (
	"fmt"
	"log"

	"github.com/teambition/rrule-go"
)

func main() {
	s, err := rrule.StrToRRuleSet(
		"DTSTART:20250719T000700pZ\n" +
			"RRULE:FREQ=DAILY;COUNT=5\n",
	)
	if err != nil {
		log.Fatalln("woy")
	}
	fmt.Println(s.All())
}
