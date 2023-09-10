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
	
        mov bx, [bp-10]
        mov cx, [bp-8]
        
        xor ax, ax
        
	xchg bx, bx

        mov al, [bp-6] ; set stdin, stdout, stderr from the stack
        mov [_stdin], al
        
        mov ax, [bp-4]
        mov [_stdout], ax
        
        mov al, [bp-2]
        mov [_stderr], al
        
        push cx ; argv
        push bx ; argc
        call _main
        pop bx ; remove argc, argv
        pop bx
        
        retf
