#asm
kernel:
    jmp _kmain
#endasm

int welcome_msg[] = "This is a test message";
int print_code = 0x0E;
int set_cursor_code = 0x0200;

void print_string(int*);
int getch();
void set_resolution(int);
void print_char(int);
void set_cursor(int, int);
void clear_screen(void);

void kmain() {
#asm
    push cs          ; save the cs reg
    pop  ds          ; use as ds also
    
#endasm
    int c;
    
    clear_screen();
    set_resolution(0x03); // 0x03 = 80x25 All modes

    print_string(welcome_msg);
    
    for(;;){
        c = getch();
        if((c>>8) == 0x1c){ // On enter, print newline
            print_char('\n');
            print_char('\r');
        } else{
            print_char(c);
        }
    }

#asm
    .k_main_loop:
    jmp .k_main_loop
#endasm
}

void clear_screen() {
#asm
    mov ax, 0x0700  ; function 07, AL=0 means scroll whole window
    mov bh, 0x07    ; character attribute = white on black
    mov cx, 0x0000  ; row = 0, col = 0
    mov dx, 0x184f  ; row = 24 (0x18), col = 79 (0x4f)
    int 0x10        ; call BIOS video interrupt
#endasm
}

void set_resolution(int mode) { // Mode = http://www.columbia.edu/~em36/wpdos/videomodes.txt
#asm
    xor ah, ah
    mov al, [bp-8]
    int 0x10
#endasm
}

void print_char(c) {
#asm
     mov ah, [_print_code]
     
     mov bx, sp
     mov al, [bp-8]
     xor bx, bx
     int 0x10
#endasm
}

void print_string(int* str) {
    char* c;
    for(c=*str ; *c!=0 ; ++c){
        print_char(*c);
    }
}

void set_cursor(int col, int row) { // BROKEN: from https://www.codeproject.com/articles/737545/writing-a-bit-dummy-kernel-in-c-cplusplus
    int val;
    
    val = (row <<= 8) | col;
#asm
    mov al, [_set_cursor_code]
    mov dl, al
    
    int 0x10
#endasm
}

int getch() {
#asm
    xor ax, ax
    int 0x16
#endasm
}
