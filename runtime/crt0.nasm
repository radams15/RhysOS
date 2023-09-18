bits 16

jmp _start

global _start

extern _stdin
extern _stdout
extern _stderr
extern _main

_start:
        mov bx, [bp+8]
        mov cx, [bp+10]

        mov al, [bp+12] ; set stdin, stdout, stderr from the stack
        mov [_stdin], al
        
        mov al, [bp+14]
        mov [_stdout], al
        
        mov al, [bp+16]
        mov [_stderr], al
        
        push cx ; argv
        push bx ; argc
        call _main
        pop bx ; remove argc, argv
        pop bx
        
        retf
