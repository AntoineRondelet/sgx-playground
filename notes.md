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

- **Measurement:** As an enclave is instantiated in a trusted environment, an accurate and protected recording of its identity is taken.
- **Attestation:** Demonstrating to other entities that a particular environment was instantiated in the correct manner.
- **Sealing:** Enabling data belonging to the trusted environment to be bound to it such that it can be restored only when the trusted environment is restored.

The enclave signature contains information that allows the Intel SGX architecture to detect whether any portion of the enclave has been tampered with. This allows an enclave to prove that it has been loaded in EPC correctly and it can be trusted.
The hardware only verifies the enclave measurement when an enclave is loaded.

##### Structure of an enclave signature

- Enclave measurement: A 256-bit hash that identifies the code and initial data to be placed inside the enclave.
- The enclave author's public key: Used to identify the enclave author
- The security version number of the enclave
- The product ID of the enclave

The author private key is used to sign the enclave. **Securing the enclave signing key is critical**

#### Attestation

- Attestation is the mechanism by which a third entity establishes that a software entity is running on a SGX enabled platform protected within an enclave prior to provisioning that software with secret and encrypted data.
- The attestation mechanism is here to ensure a party that he is communicating with a real hardware enclave and not some software emulation.

##### Local attestation (uses "report")

- Used in the case where application developers want to write enclaves which can co-operate with one another to perform some higher-level function.
- An enclave can ask the hardware to generate a **report** (a credential) which contains a cryptographic proof that the enclave exists on the platform.
- The report can be given to another enclave which can verify that the enclave report was **generated on the same platform**. To do so, a symmetric key system is used where only the enclave hardware creating the report know the key.

##### Remote attestation (uses "quote")

- Used in the case of inter-platform communications.
- An application that hosts an enclave can also ask the enclave to produce a report and then pass this report to a platform service to produce a type of credential that reflects enclave and platform state. This credential is known as **quote.**
- This quote can then be passed to entities off of the platform, and verified using techniques for Intel® EPID signature verification(EPID stands for Enhanced Privacy ID)
- **The CPU key is never directly exposed outside the platform.**
- The use of Intel EPID allows to preserve privacy of the signer, by using a group signature scheme.
- Each signer in a group has their own private key for signing, but **verifiers use the same group public key** to verify individual signatures. Therefore, users cannot be uniquely identified if signing two transactions with the same party because a **verifier cannot detect which member of the group signed a quote.**
- The group here is a group of SGX enabled platforms.
- In order to produce "quotes", Intel uses a "Quoting Enclave" (QE). It is the QE that signs measurements using the Intel EPID key (a device specific asymmetric key).

#### Secret Provisioning

- A remote entity may provision some secrets to an enclave after remote attestation has been accomplished.
- It is important to establishii secure channel with the remote enclave **AFTER** the remote attestation has been accomplished, in order to make sure that we share secrets with the good enclave (the one that has been attested).
- The verifier must also check the enclave attributes to prevent provisioning any secret to a debug enclave, for instance.
- The verifier of the remote attestation must check the identity of the signer (MRSIGNER) before provisioning any secrets. 

**Note:** Data provisioned to a debug enclave is not secret. A debug enclave’s memory is not protected by the hardware.

#### Sealing

- Allow to preserve enclave's data/state that is considered secret, across events where the enclave is destroyed.
- The secrets provisioned to an enclave are lost when the enclave is closed.
- Secrets that need to be preserved have to be stored outside the enclave boundary, encrypted by a key unique to that enclave. We say that the data is "sealed", and can be "unsealed" by the same enclave later on.

**Note:** Data can be sealed to the current enclave (by using the enclave measurement `MRENCLAVE`), or it can be sealed to the enclave author (using the author identity `MRSIGNER`). Sealing to the enclave author allows for the enclave to be updated and keep access to the sealed data, and allows enclaves from the same author to share sealed data.



## Resources

- [Resource Efficient Mining (REM)](https://www.usenix.org/system/files/conference/usenixsecurity17/sec17-zhang.pdf)
- [PoET and Hyperledger Sawtooth](https://medium.com/kokster/understanding-hyperledger-sawtooth-proof-of-elapsed-time-e0c303577ec1)
- [sgxfun](https://github.com/kudelskisecurity/sgxfun/blob/master/paper/sgxpaper.md)
