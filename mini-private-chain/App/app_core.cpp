#include <stdio.h>
#include <iostream>
#include "enclave_u.h"
#include "sgx_urts.h"
#include "enclave_manager.h"

#define TOKEN_FILE "enclave.token"
#define ENCLAVE_FILE "enclave.signed.so"

#define ETH_ADDRESS_LENGTH 42

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
    char* address_peer1 = "0x9fa3daa57360036ee16192c719e0b8856c068d3d";
    char* address_peer2 = "0xd767299db033f8b8239957d42feb3760e3d317d7";

    printf("DEBUG1\n");

    int result_init = init_enclave(&enclave_id, TOKEN_FILE, ENCLAVE_FILE);
    if (result_init < 0) {
        std::cout << "Could not initialize the enclave" << std::endl;
        return 1;
    }

    printf("DEBUG2\n");

    // Create 2 transactions to build a block in the enclave
    struct struct_transaction_t transaction1;
    transaction1.amount = "7";
    transaction1.sender = address_peer1;
    transaction1.recipient = address_peer2;

    printf("DEBUG3\n");

    struct struct_transaction_t transaction2;
    transaction2.amount = "12";
    transaction2.sender = address_peer2;
    transaction2.recipient = address_peer1;

    sgx_status_t status;
    struct_block_t result_block;

    printf("DEBUG4\n");

    status = ecall_create_block(enclave_id, &result_block, transaction1, transaction2);
    if (status != SGX_SUCCESS) {
        std::cout << "Couldn't produce a block inside the enclave" << std::endl;
    }

    printf("DEBUG5\n");

    // Display the block's characteristics
    printf("Block created: \n");
    printf("Block hash: %s\n", result_block.block_hash);
    printf("Block nonce: %d\n", result_block.nonce);
    printf("Block first TX - amount: %d\n", result_block.first_transaction.amount);
    printf("Block first TX - sender: %s\n", result_block.first_transaction.sender);
    printf("Block first TX - recipient: %s\n", result_block.first_transaction.recipient);
    printf("Block last TX - amount: %d\n", result_block.last_transaction.amount);
    printf("Block last TX - sender: %s\n", result_block.last_transaction.sender);
    printf("Block last TX - recipient: %s\n", result_block.last_transaction.recipient);

    // Destruction of the enclave once we are done with it (no more ECALLs to do)
    if(SGX_SUCCESS != sgx_destroy_enclave(enclave_id)) {
        return -1;
    }

    return 0;
}
