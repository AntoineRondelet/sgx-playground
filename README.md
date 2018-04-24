# SGX Playground

Compile the program and run it:
```bash
gcc cpu_sgx_info.c -o cpu_sgx_info
```

## Attacks

### Timing attacks

### Blind Fuzzing

### Cache attacks

#### SgxPectre

See: https://web.cse.ohio-state.edu/~zhang.834/papers/SgxPectre.pdf

## Other hardware related attacks

### Meltdown: 
See: https://meltdownattack.com/meltdown.pdf

### Spectre (Speculative execution)
See: https://spectreattack.com/spectre.pdf

The Spectre attack breaks the isolation between different applications, allowing to leak information from the kernel to user programs, as well as from virtualization hypervisors to guest systems.

- https://googleprojectzero.blogspot.co.uk/2018/01/reading-privileged-memory-with-side.html
- https://software.intel.com/sites/default/files/managed/c5/63/336996-Speculative-Execution-Side-Channel-Mitigations.pdf
- https://access.redhat.com/security/vulnerabilities/speculativeexecution

## Resources

- https://en.wikipedia.org/wiki/CPUID
- https://github.com/ayeks/SGX-hardware
- https://recon.cx/2017/montreal/resources/slides/RECON-MTL-2017-SGX_Enclave_Programming-Common%20Mistakes.pdf
- https://www.quora.com/Whats-the-difference-between-ARM-and-x86-processors
- https://cs.stanford.edu/people/eroberts/courses/soco/projects/risc/risccisc/
- https://www.felixcloutier.com/x86/CPUID.html
- https://github.com/intel/linux-sgx
- https://insujang.github.io/2017-04-14/intel-sgx-sdk-functions-for-enclave-creation/
- https://software.intel.com/en-us/sgx
- https://software.intel.com/en-us/sgx/code-samples
- https://software.intel.com/sites/default/files/332680-002.pdf
- https://eprint.iacr.org/2016/086.pdf
- https://software.intel.com/sites/default/files/managed/ae/48/Software-Guard-Extensions-Enclave-Writers-Guide.pdf
