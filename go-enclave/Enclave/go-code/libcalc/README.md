# calc: Go library to carry out arithmetic operations

## Generate a shared library from your go source code
```bash
go build -buildmode=c-shared -o libcalc.a libcalc.go
```

## Resources
- https://gist.github.com/gubatron/32f82053596c24b6bec6
- https://blog.ksub.org/bytes/2017/02/12/exploring-shared-objects-in-go/
- https://github.com/rupc/go-with-intel-sgx.git
- https://github.com/golang/go/wiki/cgo
- https://github.com/utds3lab/sgxelide
