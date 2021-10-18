package main

import (
	"fmt"
	"time"
)

func main() {
	for year := 2021; year < 2050; year++ {
		for _, month := range []int{3, 10} {
			d := time.Date(year, time.Month(month), 31, 1, 0, 0, 0, time.UTC)
			d = d.AddDate(0, 0, -int(d.Weekday()))
			fmt.Print(d.Unix(), ",\n")
		}
	}
}
