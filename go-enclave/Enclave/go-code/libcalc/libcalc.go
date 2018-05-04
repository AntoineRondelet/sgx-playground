// package name: libcalc
package main

import "C"

//export Addition
func Addition(a, b int) int {
    return a + b
}

func main() {
	// We need the main function to make possible
	// CGO compiler to compile the package as C shared library
}
