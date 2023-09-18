bits 16

global _main
extern _printf
extern _write
extern _stdout

section .text

jmp _main

print_str:
	mov ah, 0Eh
.top
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
	
	push bx

        push WORD [test_len]
        push WORD test
        push WORD [_stdout]
        call _write
        
        add sp, 6
        
        pop bx
        
	pop bp

	ret


section .data

test: db `Hello world!\r\n`, 0
test_len: dw $-test-1
axp: dd 1
stdout: db '/dev/stdout', 0
