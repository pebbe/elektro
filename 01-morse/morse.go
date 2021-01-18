package main

import (
	"fmt"
	"time"
)

var (
	data1 = "1010100011101110111000101010000000000000"
	data2 = "0001111111111000000000011111111110000000"
	data3 = "0000001111111111000000000011111111110000"

	template = fmt.Sprintf("      %c[0;7;%%dm    %c[0m                    %c[0;7;%%dm    %c[0m %c[0;7;%%dm    %c[0m\r", 27, 27, 27, 27, 27, 27)
)

func main() {
	var n1, n2, n3 int
	for i, c1 := range data1 {
		fmt.Printf("    0b%c%c%c0,\n", data3[i], data2[i], c1)
	}
	fmt.Println("    0b0001\n")
	for {
		for i, c1 := range data1 {
			if c1 == '0' {
				n1 = 0
			} else {
				n1 = 31
			}
			if data2[i] == '0' {
				n2 = 0
			} else {
				n2 = 34
			}
			if data3[i] == '0' {
				n3 = 0
			} else {
				n3 = 32
			}
			fmt.Printf(template, n1, n2, n3)
			time.Sleep(time.Millisecond * 120)
		}
	}
}
