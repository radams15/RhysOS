bits 16

jmp _start

global _start

extern _stdin
extern _stdout
extern _stderr
extern _main

extern _printf

_start:
	times 8 nop
	
        mov bx, [bp]
        mov cx, [bp+2]

        mov al, [bp+4] ; set stdin, stdout, stderr from the stack
        mov [_stdin], al
        
        mov al, [bp+6]
        mov [_stdout], al
        
        mov al, [bp+8]
        mov [_stderr], al
        
        push cx ; argv
        push bx ; argc
        call _main
        pop bx ; remove argc, argv
        pop bx
        
        retf
