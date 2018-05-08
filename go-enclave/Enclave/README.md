# Go library to call in an enclave

See: http://blog.ralch.com/tutorial/golang-sharing-libraries/

1. `go install -buildmode=shared -linkshared std`
2. `go install -buildmode=shared -linkshared calc`

