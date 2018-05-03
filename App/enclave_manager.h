#ifndef _ENCLAVE_MANAGER_H_
#define _ENCLAVE_MANAGER_H_

#include <string>

void print_error_message(sgx_status_t error);
int init_enclave(sgx_enclave_id_t* enclave_id, const std::string& launch_token_path, const std::string& enclave_file);
bool is_ecall_successful(sgx_status_t sgx_status, const std::string& err_msg, sgx_status_t ecall_return_value = SGX_SUCCESS);

#endif
