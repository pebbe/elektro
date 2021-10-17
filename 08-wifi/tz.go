package main

import (
	"fmt"
	"time"
)

func main() {
	for year := 2021; year < 2050; year++ {
		for _, month := range []int{3, 10} {
			d := time.Date(year, time.Month(month), 31, 1, 0, 0, 0, time.UTC)
			for d.Weekday() != 0 {
				d = d.AddDate(0, 0, -1)
			}
			fmt.Print(d.Unix(), ",\n")
		}
	}
}
