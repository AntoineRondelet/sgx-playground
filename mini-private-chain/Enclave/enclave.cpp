#include "string.h"
#include <stdio.h>
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
    char* amount_str = "7"; //itoa(transaction.amount, "");
    char* tx_str;
    // We store the concatenation of the amount, sender and recipient into tx_str
    // tx_str represents the "marshalling" of the Tx
    tx_str = strncat(amount_str, transaction.sender, (size_t)strlen(transaction.sender));
    tx_str = strncat(tx_str, transaction.recipient, (size_t)(transaction.recipient));
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
    // Convert transaction object to string ("marshalling" step)
    char* tx1_str = transaction_to_string(transaction1);
    char* tx2_str = transaction_to_string(transaction2);

    // Declare the nonce (can also be initialized with a random number on every "miner")
    uint32_t nonce = 0;

    sgx_sha256_hash_t block_hash;
    sgx_status_t hash_status;
    struct struct_block_t block;
    do {
        char* nonce_str = "0"; //itoa(nonce, "");

        char* block_str = build_block_string_to_hash(tx1_str, tx2_str, nonce_str);

        // Get the length of the string to hash
        size_t block_str_len = strlen(block_str);

        uint8_t ptr_uint8 = (uint8_t)atoi(block_str);
        hash_status = sgx_sha256_msg(&ptr_uint8, (uint32_t)block_str_len, &block_hash);
        if (hash_status != SGX_SUCCESS) {
            ocall_print("Could not hash data, something went wrong");
            // nonce++;

            // TODO: Remove the return for real "PoW", just kept here to test
            return block; // return empty block
        }
    } while (is_valid_hash(block_hash));

    // Create the block, now that the hash is valid
    memcpy(block.block_hash, block_hash, sizeof(block_hash));
    block.nonce = nonce;
    block.first_transaction;
    block.last_transaction;
    return block;
}
