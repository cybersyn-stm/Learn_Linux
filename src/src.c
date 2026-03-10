#include <stdio.h>

int main() {
    char result[50];
    int x;
    while (1) {
        scanf("%d", &x);
        sprintf(result, "%s%d", result, x);
        printf("result:%s\n", result);
    }
}
