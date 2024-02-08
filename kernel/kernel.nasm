bits 16

extern _main
global _start

_start:
   push si
   mov si, hello
   call print_str
   pop si

   mov ah, 0Eh
   mov al, '!'
   int 10h

   ; jmp _main
.top
   jmp .top


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

section .data
hello: db `hello world\n`
