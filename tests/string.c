#include <stdio.h>

int strlen_1(char* str) {
    int i = 0;
    char* c;

    for (c = str; *c != 0; c++)
        i++;

    return i;
}

int strncmp_1(char* a, char* b, int length) {
    int i;

    for (i = 0; i < length; i++) {
        if (a[i] != b[i])
            return 1;
    }

    return 0;
}

int strcmp_1(char* a, char* b) {
    return strncmp_1(a, b, strlen_1(a));
}

#define STREQ(a, b) (strcmp_1(a, b) == 0)

int main() {
    if (STREQ("clears", "clear")) {
        printf("Equal\n");
    } else {
        printf("Not equal\n");
    }
}
