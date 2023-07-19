bits 16

global _main
extern _printf
extern _stdout

jmp _main

section .text

_main:
	push bp
	mov bp, sp
        
        mov ax, [_stdout]
        mov [fh], ax ; fh -> dx

        mov ax, axp
        mov DWORD [axp], 7 ; write
        mov bx, [fh] ; fh
        mov cx, test ; str
        mov dx, test_len ; length
        int 21h
        
	pop bp
	ret


section .data

test: db `Hello world peas!\r\n`
test_len: dw $-test
axp: dd 1
stdout: db '/dev/stdout', 0
fh: dw 0
