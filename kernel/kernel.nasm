bits 16

global _start

extern _entry

section .text

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

	int 10h
	inc si
	jmp print_str
.print_done:
	ret


_start:
	
	mov si, msg
	mov ah, 0Eh
.lop:
	;mov al, [si]
	mov al, 'H'
	int 10h
	call _entry
	;jmp .lop

handleInterrupt21:
	ret

msg: db 'Hello world', 0ah, 0dh, 0
