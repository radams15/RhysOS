bits 16

global _start
global handleInterrupt21

jmp _start

%macro print 1
	push si
	push ax
	
	mov si, %1
	mov ah, 0Eh
	call print_str
	
	pop ax
	pop si

%endmacro

print_str:
	mov al, [si]
	cmp al, 0
	je .print_done

	int 0x10
	inc si
	jmp print_str

.print_done:
	ret

_start:
	print msg
	jmp _start

handleInterrupt21:
	ret

msg: db `Hello world\r\n`, 0
