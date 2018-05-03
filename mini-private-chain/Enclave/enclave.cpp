#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include "sgx_tcrypto.h"
#include "enclave_t.h"

#define DIFFICULTY 2 // Defines the number of 0 that need to lead in the hash to get a valid block (PoW)

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;

    if(i < 0) {
        *p++ = '-';
        i *= -1;
    }

    int shifter = i;
    do {
        ++p;
        shifter = shifter/10;
    } while(shifter);

    *p = '\0';
    do {
        *--p = digit[i%10];
        i = i/10;
    } while(i);

    return b;
}

char* transaction_to_string(struct_transaction_t transaction) {
    char* tx_str;
    ocall_print("INSIDE ENCLAVE Func transaction_to_string DEBUG 1");
    tx_str = strncat(transaction.amount, transaction.sender, (size_t)strlen(transaction.sender));
    ocall_print("INSIDE ENCLAVE Func transaction_to_string DEBUG 2");
    tx_str = strncat(tx_str, transaction.recipient, (size_t)(transaction.recipient));
    ocall_print("INSIDE ENCLAVE Func transaction_to_string DEBUG 3");
    return tx_str;
}

char* build_block_string_to_hash(char* tx1_str, char* tx2_str, char* nonce_str) {
    char* block_str;
    block_str = strncat(tx1_str, tx2_str, (size_t)strlen(tx2_str));
    block_str = strncat(block_str, nonce_str, (size_t)strlen(nonce_str));
    return block_str;
}

bool is_valid_hash(sgx_sha256_hash_t hash) {
    // TODO: verify that the hash begisn with as many zeros as indicates by the DIFFICULTY variable
    return true;
}

struct_block_t ecall_create_block(struct_transaction_t transaction1, struct_transaction_t transaction2) {
    ocall_print("INSIDE ENCLAVE DEBUG 1");
    // Convert transaction object to string ("marshalling" step)
    char* tx1_str = transaction_to_string(transaction1);
    char* tx2_str = transaction_to_string(transaction2);

    // Declare the nonce (can also be initialized with a random number on every "miner")
    uint32_t nonce = 0;

    sgx_sha256_hash_t block_hash;
    sgx_status_t hash_status;

    ocall_print("INSIDE ENCLAVE DEBUG 2");

    do {
        char* nonce_str;
        ocall_print("INSIDE ENCLAVE DEBUG 3");
        nonce_str = itoa(nonce, nonce_str);

        ocall_print("INSIDE ENCLAVE DEBUG 4");
        char* block_str = build_block_string_to_hash(tx1_str, tx2_str, nonce_str);

        // Get the length of the string to hash
        ocall_print("INSIDE ENCLAVE DEBUG 5");
        size_t block_str_len = strlen(block_str);

        ocall_print("INSIDE ENCLAVE DEBUG 6");
        hash_status = sgx_sha256_msg((uint8_t*)block_str, (uint32_t)block_str_len, &block_hash);
        if (hash_status != SGX_SUCCESS) {
            ocall_print("Could not hash data, something went wrong");
            // nonce++;

            // TODO: Remove the return for real "PoW", just kept here to test
            struct struct_block_t null_block = {};
            return null_block; // return empty block
        }
    } while (is_valid_hash(block_hash));

    // Create the block, now that the hash is valid
    struct struct_block_t res_block;

    ocall_print("INSIDE ENCLAVE DEBUG 7");
    memcpy(res_block.block_hash, block_hash, sizeof(struct_block_t));
    res_block.nonce = nonce;
    res_block.first_transaction = transaction1;
    res_block.last_transaction = transaction1;
   return res_block;
}
