#asm
export _main
_main:
    jmp _kmain

MACRO PROLOG
push bp
mov bp, sp
MEND

MACRO EPILOG
pop bp
MEND
#endasm

#include "util.h"
#include "shell.h"
#include "tty.h"
#include "fat.h"

int welcome_msg[] = "Welcome to RhysOS\r\n";

int init();

void kmain() {
#asm
    push cs          ; save the cs reg
    pop  ds          ; use as ds also
    
#endasm
    int err;

    err = init();
    
    if(err){
        print_string("\r\nError in kernel, halting!\r\n");
    }

#asm
    .k_main_loop:
    jmp .k_main_loop
#endasm
}


int init(){
    int c;
    
    clear_screen();
    set_resolution(0x03); // 0x03 = 80x25 All modes

    print_string(welcome_msg);
    
    fat_test();
    
    return 0;
}

#include "util.c"
#include "shell.c"
#include "tty.c"
#include "fat.c"
