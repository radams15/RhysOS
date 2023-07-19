bits 16

jmp _start

global _start

extern _stdin
extern _stdout
extern _stderr
extern _main

_start:
        mov bx, [bp+6]
        mov cx, [bp+8]

        mov al, [bp+10] ; set stdin, stdout, stderr from the stack
        mov [_stdin], al
        
        mov al, [bp+12]
        mov [_stdout], al
        
        mov al, [bp+14]
        mov [_stderr], al
        
        push cx ; argv
        push bx ; argc
        call _main
        pop bx ; remove argc, argv
        pop bx
        
        retf
