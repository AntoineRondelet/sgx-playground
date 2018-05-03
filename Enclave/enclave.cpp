#include "enclave_t.h"

int generate_random_number() {
    ocall_print("Processing random number generation...");
    return 42;
}

int addition(int a, int b) {
    return a + b;
}
