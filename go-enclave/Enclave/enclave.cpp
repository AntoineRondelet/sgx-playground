#include "enclave_t.h"
#include "libcalc.h"

int addition(int a, int b) {
    GoInt result;
    result = Addition((GoInt)a, (GoInt)b);
    int c_type_result = (int)result;
    return c_type_result;
}
