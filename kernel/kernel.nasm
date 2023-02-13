section .text

kernel:
    jmp _k_main

print_string:
    lodsb
    or al, al
    jz .done
    
	mov ah, [_print_al]
	
    int 0x10
    jmp print_string
.done:
    ret

_k_main:
    push cs          ; save the cs reg
    pop  ds          ; use as ds also

    mov al, [_welcome_msg]
    cbw
	mov si, ax
    call print_string

.k_main_loop:
    jmp .k_main_loop


cli
hlt

section .data

_welcome_msg db 0x0D, 0x0A, "RhysOS Boot Initialise!", 0x0D, 0x0A, 0
_print_al   dw  0x0E
