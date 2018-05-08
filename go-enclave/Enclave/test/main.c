#include "calc.h"
#include <stdio.h>

int addition(int a, int b) {
    GoInt result;

    // The logic of the addition is implemented in Golang
    result = Addition((GoInt)a, (GoInt)b);
    int c_type_result = (int)result;
    return c_type_result;
}

int main() {
  printf("This is a C application calling some Go code\n");
  int res = addition(1, 100000);
  printf("Result: %d\n", res);
  return 0;
}
