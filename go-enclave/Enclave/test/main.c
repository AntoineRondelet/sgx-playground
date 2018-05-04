#include "libcalc.h"
#include <stdio.h>

int addition(int a, int b) {
    GoInt result;
    result = Addition((GoInt)a, (GoInt)b);
    int c_type_result = (int)result;
    return c_type_result;
}

int main() {
  printf("This is a C Application.\n");
  int res = addition(1, 100000);
  printf("Result: %d\n", res);
  return 0;
}
