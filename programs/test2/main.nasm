bits 16

global _main
extern _printf

jmp _main

section .text

print_str:
	mov ah, 0Eh
.top:
	mov al, [si]
	cmp al, 0
	je .print_done

	int 10h
	inc si
	jmp .top
.print_done:
	ret

_main:
	push bp
	mov bp, sp
	
	mov si, test
	call print_str

        ;push test
        ;call _printf

	pop bp
	ret

section .data
test: db `Hello world!\r\n`, 0
