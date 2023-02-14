int print_code = 0x0E;
int set_cursor_code = 0x0200;
int zero = 0x00;
char hex_chars[] = "0123456789ABCDEF";


#asm
_clear_screen:
    PROLOG
    mov ax, 0x0700  ; function 07, AL=0 means scroll whole window
    mov bh, 0x07    ; character attribute = white on black
    mov cx, 0x0000  ; row = 0, col = 0
    mov dx, 0x184f  ; row = 24 (0x18), col = 79 (0x4f)
    int 0x10        ; call BIOS video interrupt
    EPILOG
    ret
#endasm

#asm
_set_resolution: ; Mode = http://www.columbia.edu/~em36/wpdos/videomodes.txt
    PROLOG

    xor ah, ah
    mov al, 4[bp]
    int 0x10
    EPILOG
    ret
#endasm

#asm
_print_char:
    PROLOG

    mov al, 4[bp]
    mov ah, [_print_code]
    int 0x10

    EPILOG
    
    ret
#endasm

void print_string(int* str) {
    char* c;
    for(c=*str ; *c!=0 ; ++c)
        print_char(*c);
}

void print_hex(int n) { // TODO Improve this to handle > 2 bytes.
    print_char(hex_chars[(n & 0xF0)>>4]);
    print_char(hex_chars[(n & 0x0F)>>0]);
}

void set_cursor(int col, int row) { // BROKEN: from https://www.codeproject.com/articles/737545/writing-a-bit-dummy-kernel-in-c-cplusplus
    int val;
    
    val = (row <<= 8) | col;
#asm
    mov dl, al
    mov al, [_set_cursor_code]
    
    int 0x10
#endasm
}

int getch() {
#asm
    PROLOG
    xor ax, ax
    int 0x16
    EPILOG
#endasm
}
