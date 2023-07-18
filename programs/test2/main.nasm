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

        mov ax, axp
        mov bx, stdout
        mov DWORD [axp], 8 ; open
        int 21h
        mov ax, [axp]
        mov [fh], ax ; fh -> dx

        sti
        mov ax, axp
        mov DWORD [axp], 7 ; write
        mov bx, [fh] ; fh
        mov cx, test ; str
        mov dx, test_len ; length
        int 21h

	pop bp
	ret


section .data

test: db `Hello world!\r\n`, 0
test_len: dw 13
axp: dd 1
stdout: db '/dev/stdout', 0
fh: dw 0
