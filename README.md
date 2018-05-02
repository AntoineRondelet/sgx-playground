# SGX Playground

Compile the program and run it:
```bash
gcc cpu_sgx_info.c -o cpu_sgx_info
```

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

**Note2:** We can access the symbols contained in the sgx shared object libraries we use, by running:
```bash
readelf -s $SGX_LIBRARY_PATH/libsgx_urts.so
```
For instance, which results in:
```bash
Symbol table '.dynsym' contains 24 entries:
Num:    Value          Size Type    Bind   Vis      Ndx Name
0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
1: 00000000000007b8     0 SECTION LOCAL  DEFAULT    9
2: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.2.5 (2)
4: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
5: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _Jv_RegisterClasses
6: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
7: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (2)
8: 0000000000201028     0 NOTYPE  GLOBAL DEFAULT   23 _edata
9: 000000000000095f     7 FUNC    GLOBAL DEFAULT   12 sgx_thread_setwait_untrus
10: 0000000000201030     0 NOTYPE  GLOBAL DEFAULT   24 _end
11: 0000000000000966     7 FUNC    GLOBAL DEFAULT   12 sgx_thread_wait_untrusted
12: 0000000000000943     7 FUNC    GLOBAL DEFAULT   12 sgx_oc_cpuidex
13: 000000000000094a     7 FUNC    GLOBAL DEFAULT   12 sgx_ocall
14: 000000000000092e     7 FUNC    GLOBAL DEFAULT   12 sgx_debug_unload_state_re
15: 000000000000093c     7 FUNC    GLOBAL DEFAULT   12 sgx_ecall
16: 0000000000201028     0 NOTYPE  GLOBAL DEFAULT   24 __bss_start
17: 0000000000000927     7 FUNC    GLOBAL DEFAULT   12 sgx_debug_load_state_add_
18: 00000000000007b8     0 FUNC    GLOBAL DEFAULT    9 _init
19: 0000000000000935     7 FUNC    GLOBAL DEFAULT   12 sgx_destroy_enclave
20: 0000000000000970     0 FUNC    GLOBAL DEFAULT   13 _fini
21: 0000000000000951     7 FUNC    GLOBAL DEFAULT   12 sgx_thread_set_multiple_u
22: 0000000000000958     7 FUNC    GLOBAL DEFAULT   12 sgx_thread_set_untrusted_
23: 0000000000000910    23 FUNC    GLOBAL DEFAULT   12 sgx_create_enclave

Symbol table '.symtab' contains 67 entries:
Num:    Value          Size Type    Bind   Vis      Ndx Name
0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
1: 00000000000001c8     0 SECTION LOCAL  DEFAULT    1
2: 00000000000001f0     0 SECTION LOCAL  DEFAULT    2
3: 0000000000000260     0 SECTION LOCAL  DEFAULT    3
4: 00000000000004a0     0 SECTION LOCAL  DEFAULT    4
5: 000000000000068c     0 SECTION LOCAL  DEFAULT    5
6: 00000000000006c0     0 SECTION LOCAL  DEFAULT    6
7: 00000000000006e0     0 SECTION LOCAL  DEFAULT    7
8: 00000000000007a0     0 SECTION LOCAL  DEFAULT    8
9: 00000000000007b8     0 SECTION LOCAL  DEFAULT    9
10: 00000000000007e0     0 SECTION LOCAL  DEFAULT   10
11: 0000000000000800     0 SECTION LOCAL  DEFAULT   11
12: 0000000000000810     0 SECTION LOCAL  DEFAULT   12
13: 0000000000000970     0 SECTION LOCAL  DEFAULT   13
14: 0000000000000980     0 SECTION LOCAL  DEFAULT   14
15: 00000000000009b8     0 SECTION LOCAL  DEFAULT   15
16: 0000000000000a28     0 SECTION LOCAL  DEFAULT   16
17: 0000000000200df0     0 SECTION LOCAL  DEFAULT   17
18: 0000000000200df8     0 SECTION LOCAL  DEFAULT   18
19: 0000000000200e00     0 SECTION LOCAL  DEFAULT   19
20: 0000000000200e08     0 SECTION LOCAL  DEFAULT   20
21: 0000000000200fd8     0 SECTION LOCAL  DEFAULT   21
22: 0000000000201000     0 SECTION LOCAL  DEFAULT   22
23: 0000000000201020     0 SECTION LOCAL  DEFAULT   23
24: 0000000000201028     0 SECTION LOCAL  DEFAULT   24
25: 0000000000000000     0 SECTION LOCAL  DEFAULT   25
26: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS crtstuff.c
27: 0000000000200e00     0 OBJECT  LOCAL  DEFAULT   19 __JCR_LIST__
28: 0000000000000810     0 FUNC    LOCAL  DEFAULT   12 deregister_tm_clones
29: 0000000000000850     0 FUNC    LOCAL  DEFAULT   12 register_tm_clones
30: 00000000000008a0     0 FUNC    LOCAL  DEFAULT   12 __do_global_dtors_aux
31: 0000000000201028     1 OBJECT  LOCAL  DEFAULT   24 completed.7585
32: 0000000000200df8     0 OBJECT  LOCAL  DEFAULT   18 __do_global_dtors_aux_fin
33: 00000000000008e0     0 FUNC    LOCAL  DEFAULT   12 frame_dummy
34: 0000000000200df0     0 OBJECT  LOCAL  DEFAULT   17 __frame_dummy_init_array_
35: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS urts_deploy.c
36: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS crtstuff.c
37: 0000000000000bc8     0 OBJECT  LOCAL  DEFAULT   16 __FRAME_END__
38: 0000000000200e00     0 OBJECT  LOCAL  DEFAULT   19 __JCR_END__
39: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS
40: 0000000000201020     0 OBJECT  LOCAL  DEFAULT   23 __dso_handle
41: 0000000000200e08     0 OBJECT  LOCAL  DEFAULT   20 _DYNAMIC
42: 00000000000009b8     0 NOTYPE  LOCAL  DEFAULT   15 __GNU_EH_FRAME_HDR
43: 0000000000201028     0 OBJECT  LOCAL  DEFAULT   23 __TMC_END__
44: 0000000000201000     0 OBJECT  LOCAL  DEFAULT   22 _GLOBAL_OFFSET_TABLE_
45: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
46: 0000000000000943     7 FUNC    GLOBAL DEFAULT   12 sgx_oc_cpuidex
47: 000000000000094a     7 FUNC    GLOBAL DEFAULT   12 sgx_ocall
48: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND puts@@GLIBC_2.2.5
49: 0000000000201028     0 NOTYPE  GLOBAL DEFAULT   23 _edata
50: 0000000000000935     7 FUNC    GLOBAL DEFAULT   12 sgx_destroy_enclave
51: 000000000000092e     7 FUNC    GLOBAL DEFAULT   12 sgx_debug_unload_state_re
52: 0000000000000970     0 FUNC    GLOBAL DEFAULT   13 _fini
53: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
54: 000000000000095f     7 FUNC    GLOBAL DEFAULT   12 sgx_thread_setwait_untrus
55: 000000000000093c     7 FUNC    GLOBAL DEFAULT   12 sgx_ecall
56: 0000000000201030     0 NOTYPE  GLOBAL DEFAULT   24 _end
57: 0000000000000951     7 FUNC    GLOBAL DEFAULT   12 sgx_thread_set_multiple_u
58: 0000000000201028     0 NOTYPE  GLOBAL DEFAULT   24 __bss_start
59: 0000000000000966     7 FUNC    GLOBAL DEFAULT   12 sgx_thread_wait_untrusted
60: 0000000000000958     7 FUNC    GLOBAL DEFAULT   12 sgx_thread_set_untrusted_
61: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _Jv_RegisterClasses
62: 0000000000000927     7 FUNC    GLOBAL DEFAULT   12 sgx_debug_load_state_add_
63: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
64: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@@GLIBC_2.2
65: 00000000000007b8     0 FUNC    GLOBAL DEFAULT    9 _init
66: 0000000000000910    23 FUNC    GLOBAL DEFAULT   12 sgx_create_enclave
```

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
