#include <cstdio>
#include <cstring>
#include "enclave_manager.h"
/**
 * If we look at the include folder of the sgx sdk, sgx_urts.h is the file that
 * contains the declarations of the functions "sgx_create_enclave" and "sgx_destroy_enclave"
 * Note: This file is linked at compilation time
 */
#include "sgx_urts.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// Check error conditions for loading enclave
void print_error_message(sgx_status_t status) {
    printf("SGX error code: %d\n", status);
}

int initialize_enclave(sgx_enclave_id_t* enclave_id, const std::string& launch_token_path, const std::string& enclave_file) {
    /**
     * To create an enclave, we call the function "sgx_create_enclave"
     * sgx_create_enclave(
     *   const char *file_name,
     *   const int debug,
     *   sgx_launch_token_t *launch_token,
     *   int *launch_token_updated,
     *   sgx_enclave_id_t *enclave_id,
     *   sgx_misc_attribute_t *misc_attr);
     * The file name represents the enclave source code (built as a shared object)
     * The sgx_create_enclave function loads the enclave source code (.so) in protected memory.
     * When loading the enclave for the first time, we get a launch token (launch_token parameter).
     * The launch_token can be saved into a file for further launches of the same enclave.
     * To unload an enclave, we should use the function "sgx_destroy_enclave".
     */
    const char* token_path = launch_token_path.c_str();
    sgx_launch_token_t token = {0};
    sgx_status_t status = SGX_ERROR_UNEXPECTED;
    int updated = 0;

    /**
     * Verify if any launch_token exists for this enclave.
     * If yes, then retrieve it, otherwise, create the enclave and get a new token
     */
    FILE* fp = fopen(token_path, "rb");
    if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
        printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
    }

    if (fp != NULL) {
        /* read the token from saved file */
        size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
        if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
            /* if token is invalid, clear the buffer */
            memset(&token, 0x0, sizeof(sgx_launch_token_t));
            printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
        }
    }

    /**
     * Call sgx_create_enclave to initialize an enclave instance
     */
    status = sgx_create_enclave(enclave_file.c_str(), SGX_DEBUG_FLAG, &token, &updated, enclave_id, NULL);
    if (status != SGX_SUCCESS) {
        print_error_message(status);
        if (fp != NULL) {
            fclose(fp);
        }
        return -1;
    }

    /**
     * Save the launch token for further use
     */
    if (updated == FALSE || fp == NULL) {
        /* if the token is not updated, or file handler is invalid, do not perform saving */
        if (fp != NULL) {
            fclose(fp);
        }
        return 0;
    }

    /*
     * Reopen the file with write capablity
     */
    fp = freopen(token_path, "wb", fp);
    if (fp == NULL) return 0;
    size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
    if (write_num != sizeof(sgx_launch_token_t)) {
        printf("Warning: Failed to save launch token to \"%s\".\n", token_path);
    }
    fclose(fp);
    return 0;
}

bool is_ecall_successful(
        sgx_status_t sgx_status, const std::string& err_msg,
        sgx_status_t ecall_return_value)
{
    if (sgx_status != SGX_SUCCESS || ecall_return_value != SGX_SUCCESS) {
        printf("%s\n", err_msg.c_str());
        print_error_message(sgx_status);
        print_error_message(ecall_return_value);
        return false;
    }
    return true;
}
