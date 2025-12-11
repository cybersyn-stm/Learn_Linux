#include "add.h"
#include "sub.h"
#include <stdio.h>

int main() {
    int a = 15;
    int b = 10;
    int c;

    c = add(a, b);
    printf("a + b = %d\n", c);
    c = sub(a, b);
    printf("a - b = %d\n", c);

    return 0;
}
