# Notes from SGX papers and documentation

### Idea of SGX

- Trusted code runs in an environment that is private to the rest of the application.
- A specialized hardware component can create an attestation that some particular trusted code was set up correctly in a protected environment.

### Intel Attestation Service access generates an overhead

- We need to contact the IAS to verify attestations.
- Currently there is no way to verify attestations locally.
- The IAS is a public web service that receives SGX attestations and responds with verification results.
- Requests are submitted to the IAS over HTTPS; a response is a signed “report” indicating the validation status of the queried platform.
- An IAS access is required for every block verification by every blockchain participant (because a report is not cryptographically linked with its corresponding request. Thus, the report is only trustworthy for the client initiating the HTTPS request).

### PoET and SGX

The idea of PoET is:
1. Each participant in the blockchain network waits a random amount of time.
2. The first participant to finish waiting gets to be leader for the new block.

Since SGX allow applications to run trusted code in a protected environment, we can make sure that the code is trusted and that no one can tamper with it in order to, intentionally, choose short wait times and be elected leader at every block.
Thus, **the trustworthyness of the hardware is what keeps the lottery (leader election) fair.**

### Enclave programming
From [Intel's SGX enclave guide](https://software.intel.com/sites/default/files/managed/ae/48/Software-Guard-Extensions-Enclave-Writers-Guide.pdf)

#### File format

- DLL - Windows OS
- Dynamic Library - OS X
- Shared Object - Linux OS

An enclave library file contains trusted code and data sections corresponding to the functions and/or methods as well as the variables and/or objects implemented in the shared library. Code and data sections are loaded into protected memory (EPC) when the enclave is created.

In an enclave file, there is also an Intel SGX specific data structure, the enclave metadata. The metadata is not loaded into EPC. Instead, it is used by the untrusted loader to determine how to properly load the enclave in EPC.

The metadata also contains the enclave signature, which is a vital certificate of authenticity and origin of an enclave.

#### Trusted Computing Base (TCB)

The first step whem designing a SGX-enabled application is to identitify the data that needs to be protected, their data structures, and the set of instructions that operate on them.

**Note:** The code in an enclave is no different than code that exists as part of a regular application.
However, **enclave code is loaded in a special way** such that once the enclave has been initialized, privileged code and the rest of the untrusted application **cannot directly read data that resides in the protected environment, or change the behavior of the code within the enclave without detection.**

- We want to keep the TCb size as small as possible, since the smaller the TCB, the smaller the attack surface i- We want to keep the TCb size as small as possible, since the smaller the TCB, the smaller the attack surface is.

#### ECall interface function

- Used by untrusted application (untrusted code) to call functions running in an enclave.
- A "ECall" call is followed by a "OCall" which returns the result of the execution of the function called by ECall. When a OCall returns, the trusted function that made the call outside the enclave continues its execution.

**Note:** An enclave has not control over which ECall is executed, or the order in which they are executed !

#### Enclave inputs and outputs

- Enclave inputs can be observed by and modified by untrusted code.
- The enclave code must **always** check ECall input parameters and OCall return values.
- The enclave code must **never** trust information coming from the untrusted domain.
- In the case of inputs passed by reference, only the reference (pointer) is inside the enclave. The value referenced can be outside, and changed at any time (by an attacker potentially).

#### Enclave signature

**Measurement:** As an enclave is instantiated in a trusted environment, an accurate and protected recording of its identity is taken.
**Attestation:** Demonstrating to other entities that a particular environment was instantiated in the correct manner.
**Sealing:** Enabling data belonging to the trusted environment to be bound to it such that it can be restored only when the trusted environment is restored.



## Resources

- [Resource Efficient Mining (REM)](https://www.usenix.org/system/files/conference/usenixsecurity17/sec17-zhang.pdf)
- [PoET and Hyperledger Sawtooth](https://medium.com/kokster/understanding-hyperledger-sawtooth-proof-of-elapsed-time-e0c303577ec1)
- [sgxfun](https://github.com/kudelskisecurity/sgxfun/blob/master/paper/sgxpaper.md)
