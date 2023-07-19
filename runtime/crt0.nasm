bits 16

;jmp _start

global _start

extern _stdin
extern _stdout
extern _stderr
extern _main

_start:
        mov al, [bp+10] ; set stdin, stdout, stderr from the stack
        mov [_stdin], al
        
        mov al, [bp+12]
        mov [_stdout], al
        
        mov al, [bp+14]
        mov [_stderr], al
        
        
        push bx
        
        push 0
        push 0
        call _main
        pop bx ; remove argc, argv
        pop bx
        
        pop bx
        
        retf
