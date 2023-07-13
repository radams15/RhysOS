int main();
void entry() { main(); }

int interrupt (int number, int AX, int BX, int CX, int DX);

void printc(char c) {
    interrupt(0x10, 0x0E00 + c, 0, 0, 0);
}

void print(char* str) {
    char* c;
    for(c = str ; *c != 0 ; c++)
        printc(*c);
}

int main() {
    print("Hello world!\r\n");
    print("Hello world 2!\r\n");

    for(;;) {}
}
