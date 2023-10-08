#include <stdio.h>

char hex_chars[] = "0123456789ABCDEF";

void print_hex(int n, int len) {
    int index;
    int shift = len * 4;
    int and   = 0xF << (len * 4);
    printf("0x");

    while (shift > 0) {
        and >>= 4;
        shift -= 4;

        index = hex_chars[(n & and) >> shift];

        printf("%c", index);
    }

    printf("\n");
}

int main() {
    print_hex(0xabcdef, 6);
}
