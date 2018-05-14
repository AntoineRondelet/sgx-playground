# SGX Playground

In order to inspect the properties of you CPU and see if it supports SGX, run:
```bash
gcc cpu_sgx_info.c -o cpu_sgx_info
./cpu_sgx_info
```

Or go directly check here: https://github.com/ayeks/SGX-hardware

## Enclave development

### Prerequisites

- See: https://github.com/intel/linux-sgx.git
- For a full reference on how to use SGX on Linux, see the [Intel SGX Developer Reference](https://download.01.org/intel-sgx/linux-2.1/docs/Intel_SGX_Developer_Reference_Linux_2.1_Open_Source.pdf)

In order to use Intel SGX:
- The CPU must support Intel SGX instructions
- The BIOS must support Intel SGX, and SGX must be enabled
- The SGX PSW (Platform Software) must be installed

1. Go on: https://01.org/intel-software-guard-extensions/downloads, and download the SGX installers for your OS.
2. Install the SGX driver, the SDK and the PSW (the commands below are for Ubuntu Desktop 64bit)
```bash
mkdir ./sgx-binaries && cd ./sgx-binaries
wget https://download.01.org/intel-sgx/linux-2.1.2/ubuntu64-desktop/sgx_linux_x64_driver_1bf506e.bin
wget https://download.01.org/intel-sgx/linux-2.1.2/ubuntu64-desktop/sgx_linux_x64_psw_2.1.102.43402.bin
wget https://download.01.org/intel-sgx/linux-2.1.2/ubuntu64-desktop/sgx_linux_x64_sdk_2.1.102.43402.bin

chmod +x ./*.bin
sudo ./sgx_linux_x64_driver_1bf506e.bin
./sgx_linux_x64_psw_2.1.102.43402.bin
sudo ./sgx_linux_x64_sdk_2.1.102.43402.bin

HERE=$(pwd)
source $HERE/sgxsdk/environment
```
3. If you want to test enclave code, you can test with the code samples from the [linux-sgx](https://github.com/intel/linux-sgx.git) github repo.

### Architecture of an enclave application

- **Untrusted** part: Starts the enclave, and interacts with the "outside world"
- **Trusted** part: Executes trusted code, and uses secrets

The **untrusted** code can do calls to the **trusted** code by using the `ECALL` instruction.
Similarly, the **trusted** code can return a result to the **untrusted** code by using the `OCALL` instruction.

### Makefile

Here are a few explanations about common instructions seen in Makefiles used to compile enclaves programs:
1. Locate the SGX SDK: `SGX_SDK ?= /opt/intel/sgxsdk` sets the `SGX_SDK` environment variable to `/opt/intel/sgxsdk` if this environment variable does not exists (does not have any value).
2. Define the architecture: `SGX_ARCH ?= x64`
3. Define the SGX Mode you want to use: `SGX_MODE ?= [MODE]`. If we set`[MODE]` to `HW`, then we get a hardware enclave. In order to run a SGX simulation, we can set `[MODE]` to `SIM`.
4. Set the configuration modes: `SGX_DEBUG = 1` activates the debug mode, and enables the macro `DEBUG`. `SGX_PRERELEASE = 1` activates the prerelease mode, and enables the macros `NDEBUG` and `EDEBUG`. Finally, `SGX_RELEASE = 1` activates the release mode, and enables the `NDEBUG` macro.
5. Locate the libraries: `SGX_LIBRARY_PATH := $(SGX_SDK)/lib64`
Here is a reminder about [linux libraries](http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html) (`.a` and `.so` files)
6. Define the include path to the sdk files to include during compilation: `INCLUDE_PATH := -I$(SGX_SDK)/include`

**Note1:** Files from the include folder of the SDK contain instructions with placeholders for compiler or system extensions to the language. See https://stackoverflow.com/questions/34495206/whats-a-macro-before-a-function-or-a-class-in-c for more details.

**Note2:** We can access the symbols contained in the sgx shared object libraries we by running the following command for instance:
```bash
readelf -s $SGX_LIBRARY_PATH/libsgx_urts.so
```

### Tooling

- **Edger8r:** Generates the interfaces between the untrusted components and enclaves.
edger8r takes the `.edl` file as an argument and generates the pairs of `_t` and `_u` files referring to the trusted and untrusted
proxies and bridges. Trusted proxies correspond to OCALLs (called by the enclave), and untrusted proxy functions correspond to ECALLs (called by the application).
- **Signing tool:** Generates and adds enclave metadata to the enclave image (the enclave signature is included into the metadata).
Once an enclave is signed, any modifications to the enclave file can be detected.
The signing tool (`sgx_sign`) takes the enclave configuration file (XML file) as an input to create the signature and metadata of the enclave (see p60 of the [intel documentation](https://download.01.org/intel-sgx/linux-2.1/docs/Intel_SGX_Developer_Reference_Linux_2.1_Open_Source.pdf))

### Enclave development process

1. Define the interface between the untrusted application and the enclave in the EDL file (see p37 of the [intel documentation](https://download.01.org/intel-sgx/linux-2.1/docs/Intel_SGX_Developer_Reference_Linux_2.1_Open_Source.pdf))
The enclave EDL represents the interafce between trusted and untrusted execution environments. The EDL file is [here](https://github.com/AntoineRondelet/sgx-playground/blob/master/Enclave/enclave.edl)
2. Implement the application logic and the enclave functions (See the cpp files [enclave.cpp](https://github.com/AntoineRondelet/sgx-playground/blob/master/Enclave/enclave.cpp) for the enclave and [app_core](https://github.com/AntoineRondelet/sgx-playground/blob/master/App/app_core.cpp), and [enclave_manager.cpp](https://github.com/AntoineRondelet/sgx-playground/blob/master/App/enclave_manager.cpp) for the untrusted part of the application)
3. Build the project (app and enclave), by using the edger8r and signing tools. In order to build the test application, run: `make`. The Makefile takes cares to use the `edger8r` and `sgx_sign` tools in order to generate the `*_u.h`, `*_u.c` files representing the interface for doing ECALLs, and the `*_t.h` and `*_t.c` files used by the enclave to communicate with the untrusted environment of the appliaction (OCALLs).
4. Run the project, by executing the compiled binary resulting from the compilation.

**Note1:** In order to generate a key for the enclave, run:
```bash
openssl genpkey -out enclave_private.pem -algorithm rsa -outform PEM -pkeyopt rsa_keygen_bits:3072 -pkeyopt rsa_keygen_pubexp:3
```
Then you can generate the coresponding public key, by running:
```bash
openssl rsa -in enclave_private.pem -pubout > enclave_public.pub
```
And finally, you get check the modulus and the exponent of the public key by running:
```bash
openssl rsa -pubin -in enclave_public.pub -text -noout
```

**Note2:** If you want to run the program on the hardware mode (set `SGX_MODE=HW`), then, depending on your OS, a problem might occur when you launch the executable. Make sure to have `libprotobuf.so.9` somewhere in your libraries. (You can install it by having a look [here](https://altlinux.pkgs.org/sisyphus/classic-x86_64/libprotobuf-compat9-2.6.1-alt2.x86_64.rpm.html))

**Note3:** Some explanations about C and C++ interactions can be found [here](https://stackoverflow.com/questions/3789340/combining-c-and-c-how-does-ifdef-cplusplus-work)

## Attacks

### Timing attacks

### Blind Fuzzing

### Cache attacks

#### SgxPectre
See: https://web.cse.ohio-state.edu/~zhang.834/papers/SgxPectre.pdf

> When branch prediction of the enclave code can be influenced by programs outside the enclave, the control flow of the enclave program can be temporarily altered to execute instructions that lead to observable cache-state changes. An adversary observing such changes can learn secrets inside the enclave memory or its internal registers, thus completely defeating the confidentiality guarantee offered by SGX.

## Other hardware related attacks

### Meltdown: 
See: https://meltdownattack.com/meltdown.pdf

> Meltdown exploits side effects of out-of-order execution on modern processors to read arbitrary kernel-memory locations including personal data and passwords.

### Spectre (Speculative execution)
See: https://spectreattack.com/spectre.pdf

> Speculative logic is unfaithful in how it executes, can access to the victim’s memory and registers, and can perform operations with measurable side effects.

> Spectre attacks involve inducing a victim to speculatively perform operations that would not occur during correct program execution and which leak the victim’s confidential information via a side channel to the adversary.

- https://googleprojectzero.blogspot.co.uk/2018/01/reading-privileged-memory-with-side.html
- https://software.intel.com/sites/default/files/managed/c5/63/336996-Speculative-Execution-Side-Channel-Mitigations.pdf
- https://access.redhat.com/security/vulnerabilities/speculativeexecution

## Resources

- https://en.wikipedia.org/wiki/CPUID
- https://github.com/ayeks/SGX-hardware
- https://recon.cx/2017/montreal/resources/slides/RECON-MTL-2017-SGX_Enclave_Programming-Common%20Mistakes.pdf
- https://cs.stanford.edu/people/eroberts/courses/soco/projects/risc/risccisc/
- https://www.felixcloutier.com/x86/CPUID.html
- https://insujang.github.io/2017-04-14/intel-sgx-sdk-functions-for-enclave-creation/
- https://software.intel.com/en-us/sgx
- https://software.intel.com/sites/default/files/332680-002.pdf
- https://eprint.iacr.org/2016/086.pdf
- https://software.intel.com/sites/default/files/managed/ae/48/Software-Guard-Extensions-Enclave-Writers-Guide.pdf
- https://www.blackhat.com/docs/us-16/materials/us-16-Aumasson-SGX-Secure-Enclaves-In-Practice-Security-And-Crypto-Review-wp.pdf
- https://github.com/google/prochlo/tree/master/prochlo_stash_shuffler
