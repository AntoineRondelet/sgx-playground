#include <stdio.h>
#include <iostream>
/**
 * enclave_u.h is generated by the Edger8r tool.
 * The generation of edge routines is done by reading the EDL file.
 * The suffix "_u" means "untrusted", and is used to make
 * enclave calls from the untrusted part of the application.
 * The enclave_u.h file gives an interface to the untrusted
 * part of the application to interact with the trusted part in the enclave
 */
#include "enclave_u.h"
#include "sgx_urts.h"
#include "enclave_manager.h"

#define TOKEN_FILE "enclave.token"
#define ENCLAVE_FILE "enclave.signed.so"

// Global enclave ID
sgx_enclave_id_t enclave_id = 0;

/**
 * Implementation of the untrusted (the OCALLs) part of the enclave.edl file
 */
void ocall_print(const char* str) {
    printf("%s\n", str);
}

/**
 * Main program
 */
int main(int argc, char const *argv[]) {
    int result_init = init_enclave(&enclave_id, TOKEN_FILE, ENCLAVE_FILE);
    if (result_init < 0) {
        std::cout << "Could not initialize the enclave" << std::endl;
        return 1;
    }
    /**
     * After initializing the enclave, we can do a bunch of ECALL (Enclave calls)
     * When we are done with the ECALLs, we can destroy the enclave
     * By using the "sgx_destroy_enclave" function
     */

    int ptr;
    /**
     * Call the generate_random_number function of the enclave
     * Note, that when we run: sgx_edger8r enclave.edl, we obtain a function with the signature:
     * sgx_status_t generate_random_number(sgx_enclave_id_t eid, int* retval);
     */
    sgx_status_t status = generate_random_number(enclave_id, &ptr);
    std::cout << status << std::endl;
    if (status != SGX_SUCCESS) {
        std::cout << "Something went wrong when calling the generate_random_number function" << std::endl;
    }
    // The value of the pointer ptr is set in the enclave
    printf("Random number: %d\n", ptr);

    // Seal the random number stored at the address ptr
    size_t sealed_size = sizeof(sgx_sealed_data_t) + sizeof(ptr);
    uint8_t* sealed_data = (uint8_t*)malloc(sealed_size);

    sgx_status_t ecall_status;
    status = seal(
            enclave_id,
            &ecall_status,
            (uint8_t*)&ptr,
            sizeof(ptr),
            (sgx_sealed_data_t*)sealed_data,
            sealed_size);
    if (!is_ecall_successful(status, "Sealing failed", ecall_status)) {
        return 1;
    }

    int unsealed;
    status = unseal(
            enclave_id,
            &ecall_status,
            (sgx_sealed_data_t*)sealed_data,
            sealed_size,
            (uint8_t*)&unsealed,
            sizeof(unsealed));
    if (!is_ecall_successful(status, "Unsealing failed", ecall_status)) {
        return 1;
    }
    std::cout << "Seal round trip success! Received back: " << unsealed << std::endl;

    int result_addition = 10;
    int first_term = 12;
    int second_term = 34;
    /**
     * Perform an addition in the enclave
     * See the function's signature generated in the enclave_u.h file
     * sgx_status_t addition(sgx_enclave_id_t eid, int* retval, int a, int b);
     */
    printf("Value of the result_addition variable BEFORE call to enclave: %d\n", result_addition);
    status = addition(enclave_id, &result_addition, first_term, second_term);
    if (status != SGX_SUCCESS) {
        std::cout << "The addition call in the enclave failed" << std::endl;
    }
    printf("Value of the result_addition variable AFTER call to enclave: %d\n", result_addition);

    // Destruction of the enclave once we are done with it (no more ECALLs to do)
    if(SGX_SUCCESS != sgx_destroy_enclave(enclave_id)) {
        return -1;
    }

    return 0;
}
