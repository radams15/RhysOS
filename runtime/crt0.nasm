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
	
        mov bx, [bp+4]
        mov cx, [bp+6]

        mov al, [bp+8] ; set stdin, stdout, stderr from the stack
        mov [_stdin], al
        
        mov al, [bp+10]
        mov [_stdout], al
        
        mov al, [bp+12]
        mov [_stderr], al
        
        push cx ; argv
        push bx ; argc
        call _main
        pop bx ; remove argc, argv
        pop bx
        
        retf
