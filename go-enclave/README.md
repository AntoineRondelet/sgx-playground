# Invoke Go code in an enclave

The `go-enclave` repo is an attempt to call Golang functions from C++ code running in an SGX enclave.
The purpose of this is to build an application in Golang, and use `cgo` in order to generate a `c-shared` library, link this library to the enclave C code, and call the Golang logic from the enclave's code.

**Important:** This does not work ! Despite several attempts, I couldn't manage to make it work.
The compilation is successful, but I cannot sign the enclave, and thus, I cannot run it.

## Error obtained at compilation time

```bash
9:25:30 ~/D/A/s/go-enclave ❯❯❯ make
GEN  =>  App/enclave_u.c
CC   <=  App/enclave_u.c
CXX  <=  App/app_core.cpp
CXX  <=  App/enclave_manager.cpp
LINK =>  playground_app
INCLUDE FILES
GEN  =>  Enclave/enclave_t.c
CC   <=  Enclave/enclave_t.c
CXX  <=  Enclave/enclave.cpp
LINK =>  enclave.so
<EnclaveConfiguration>
<ProdID>0</ProdID>
<ISVSVN>0</ISVSVN>
<StackMaxSize>0x50000</StackMaxSize>
<HeapMaxSize>0x100000</HeapMaxSize>
<!--TCS: Thread Control Structure-->
<TCSNum>10</TCSNum>
<TCSPolicy>1</TCSPolicy>
<DisableDebug>0</DisableDebug>
<MiscSelect>0</MiscSelect>
<MiscMask>0xFFFFFFFF</MiscMask>
</EnclaveConfiguration>
[check_symbol_table elfparser.cpp:231] symbol 'Addition' is undefined
The input enclave file is not correct.
Error happened while signing the enclave.
Makefile:221: recipe for target 'enclave.signed.so' failed
make: *** [enclave.signed.so] Error 255
>>
```

This error seems to be coherent with the **Caution** mention of the p92 of the Intel Developer Reference for Linux (see: p92 https://download.01.org/intel-sgx/linux-2.1/docs/Intel_SGX_Developer_Reference_Linux_2.1_Open_Source.pdf), which states:

> Do not link the enclave with any untrusted library including C/C++ standard libraries. This action will either fail the enclave signing process or cause a runtime failure due to the use of restricted instructions.

While this does not seem possible (and certainly not secure), any hack to make this work is more than welcomed :)
