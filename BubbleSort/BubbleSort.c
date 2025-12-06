#include <stdio.h>
#define arraySize 15
int array[arraySize] = {
    1, 42, 23, 32, 66, 421, 56, 7864, 123166, 43123, 566, 213, 23, 55, 677,
};
int main() {
    int temp;
    for (int n = 0; n < arraySize; n++) {
        printf("%d,", array[n]);
    }
    printf("\n");
    for (int n = 0; n < arraySize; n++) {
        for (int j = 0; j < arraySize - 1 - n; j++) {
            if (array[j] > array[j + 1]) {
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
    for (int n = 0; n < arraySize; n++) {
        printf("%d,", array[n]);
    }
    printf("\n");
}
