bits 16

;jmp _start

global _start

extern _stdin
extern _stdout
extern _stderr
extern _main

_start:
        mov DWORD [_stdin], 0
        mov DWORD [_stdout], 1
        mov DWORD [_stderr], 2
        
        push bx
        
        push 0
        push 0
        call _main
        pop bx
        pop bx
        
        pop bx
        
        retf
