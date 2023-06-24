bits 16

extern _cstart_
global entry

jmp entry

%macro print 1 ; destroys si, ax, dx
	push si
	push ax
	push ds
	
	mov ax, 0
	mov ds, ax
	
	mov si, %1
	call print_str
	
	pop ds
	pop ax
	pop si
%endmacro

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

entry:
	;jmp _cstart_
	
	mov ah, 0Eh
	mov al, 'a'
	int 10h
	mov al, 'b'
	int 10h
	
.loop:
	mov si, 0x1028
	xor ax, ax
	call print_str
	mov al, 0xd
	int 10h
	jmp .loop
	
	jmp $

;section DATA
teststr: db 'test', 0xa, 0xd 0

segment stack stack
resb 1024
stacktop: