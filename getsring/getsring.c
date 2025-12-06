#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *string;
char *get_string(char *name);
int main(void) {
    string = get_string("What's your name? ");
    printf("Hello %s!\n", string);
    return 0;
}
char *get_string(char *str) {
    int buffSize = 256;
    char *buffer = malloc(buffSize * sizeof(char));
    if (buffer == NULL) {
        printf("error");
        return 0;
    }
    printf("%s", str);
    fgets(buffer, buffSize, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    return buffer;
}
