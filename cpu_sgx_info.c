#include <stdio.h>

// Note: The CPUID instruction is not sufficient to detect the usability of Intel SGX on a platform.
// It can report whether or not the processor supports the Intel SGX instructions, but Intel SGX usability depends on both the BIOS settings and the PSW.
// Applications that make decisions based solely on CPUID enumeration run the risk of generating a #GP or #UD fault at runtime.
// See: https://software.intel.com/en-us/articles/properly-detecting-intel-software-guard-extensions-in-your-applications

// inline instructs the compiler to attempt to embed the function content into the calling code instead of executing an actual call
// This is a performance optimization "request to the compiler", which can make a big perf difference for functions that are called frequently
// static refers to the scope. In C it means that the function/variable can only be used within the same translation unit.
static inline void cpuid_call(unsigned int *eax, unsigned int *ebx, unsigned int *ecx, unsigned int *edx) {
    // The CPUID instruction takes no parameters as CPUID implicitly uses the EAX register to determine the main category of information returned

    // The asm keyword allows you to embed assembler instructions within C code
    // Accessing raw CPUID information. The CPUID opcode is a processor supplementary instruction
    // for the x86 architecture allowing software to discover details of the processor

    // Volatile tells the compiler not to optimize anything that has to do with the volatile variable.
    // There is only one reason to use it: When you interface with hardware.
    asm volatile("cpuid": "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx) : "0" (*eax), "2" (*ecx));
}

int main(int argc, char **argv) {
    // This programm prints some CPUID information and tests the SGX support of the CPU
    // See: https://en.wikipedia.org/wiki/CPUID for details about the CPUID interpretation
    unsigned eax, ebx, ecx, edx;

    // EAX=1: Processor Info and Feature Bits
    eax = 1;
    printf("==> Processor information (EAX=01H): <==\n");
    cpuid_call(&eax, &ebx, &ecx, &edx);

    printf("EAX: %x EBX: %x ECX: %x EDX: %x\n", eax, ebx, ecx, edx);
    // Stepping: Bit 3:0
    printf("Stepping: %d\n", eax & 0xF);
    // Model: Bit 7:4
    printf("Model: %d\n", (eax >> 4) & 0xF);
    // Family: Bit 11:8
    printf("Family: %d\n", (eax >> 8) & 0xF);
    // Processor Type: Bit 13:12
    printf("Processor type: %d\n", (eax >> 12) & 0x3);
    // Extended Model: 19:16
    printf("Extended model: %d\n", (eax >> 16) & 0xF);
    // Extended Family: 27:20
    printf("Extended family: %d\n", (eax >> 20) & 0xFF);

    // Check if SMX is enabled (6th bit of ECX)
    // https://en.wikipedia.org/wiki/Trusted_Execution_Technology
    // http://linasm.sourceforge.net/docs/instructions/smx.php
    printf("SMX: %d\n", (ecx >> 6) & 1);

    // EAX=7, ECX=0: Extended Features
    eax = 7;
    ecx = 0;
    printf("\n ==> Extended feature bits (EAX=07H, ECX=0H) <==\n");
    cpuid_call(&eax, &ebx, &ecx, &edx);
    printf("EAX: %x EBX: %x ECX: %x EDX: %x\n", eax, ebx, ecx, edx);

    // Bit 2 of EBX indicates the SGX (Software Guard Extension) feature
    printf("SGX available: %d\n", (ebx >> 2) & 0x1);

    // CPUID Leaf 12H, Sub-Leaf 0 Enumeration of Intel SGX Capabilities (EAX=12H, ECX=0H)
    eax = 0x12;
    ecx = 0;
    printf("\n ==> CPUID Leaf 12H, Sub-Leaf 0 of Intel SGX Capabilities (EAX=12H, ECX=0H) <== \n");
    cpuid_call(&eax, &ebx, &ecx, &edx);
    printf("EAX: %x EBX: %x ECX: %x EDX: %x\n", eax, ebx, ecx, edx);

    printf("SGX 1 supported: %d\n", eax & 0x1);
    printf("SGX 2 supported: %d\n", (eax >> 1) & 0x1);
    printf("MaxEnclaveSize_Not64: %x\n", edx & 0xFF);
    printf("MaxEnclaveSize_64: %x\n", (edx >> 8) & 0xFF);

    // CPUID Leaf 12H, Sub-Leaf 1 Enumeration of Intel SGX Capabilities (EAX=12H, ECX=1H)
    eax = 0x12;
    ecx = 1;
    printf("\n ==> CPUID Leaf 12H, Sub-Leaf 1 of Intel SGX Capabilities (EAX=12H, ECX=1H) <== \n");
    cpuid_call(&eax, &ebx, &ecx, &edx);
    printf("EAX: %x EBX: %x ECX: %x EDX: %x\n", eax, ebx, ecx, edx);

    int i;
    for (i=2; i<10; i++) {
        // CPUID Leaf 12H, Sub-Leaf i Enumeration of Intel SGX Capabilities (EAX=12H, ECX=i)
        printf("\n ==> CPUID Leaf 12H, Sub-Leaf %d of Intel SGX Capabilities (EAX=12H, ECX=%d) <== \n", i, i);
        eax = 0x12;
        ecx = i;
        cpuid_call(&eax, &ebx, &ecx, &edx);
        printf("EAX: %x EBX: %x ECX: %x EDX: %x\n", eax, ebx, ecx, edx);
    }
}

// Resources:
// https://github.com/ayeks/SGX-hardware
// https://recon.cx/2017/montreal/resources/slides/RECON-MTL-2017-SGX_Enclave_Programming-Common%20Mistakes.pdf
// https://www.quora.com/Whats-the-difference-between-ARM-and-x86-processors
// https://cs.stanford.edu/people/eroberts/courses/soco/projects/risc/risccisc/
// https://www.felixcloutier.com/x86/CPUID.html
