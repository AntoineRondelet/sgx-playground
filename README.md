# SGX Playground

Compile the program and run it:
```bash
gcc cpu_sgx_info.c -o cpu_sgx_info
```

## Enclave development

### Prerequisites

See: https://github.com/intel/linux-sgx.git

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

**Note:** Files from the include folder of the SDK contain instructions with placeholders for compiler or system extensions to the language. See https://stackoverflow.com/questions/34495206/whats-a-macro-before-a-function-or-a-class-in-c for more details.

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
