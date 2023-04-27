global _main

; Works but needs to be loaded into memory for the string pointers to work!

_main:
	push bp
	
	mov ax, 0 ; interrupt num
	mov bx, msg
	mov cx, 0
	mov dx, 0
	
	int 0x21
	
	mov ax, 0x1 ; return 0x1
	
	pop bp
	ret

msg db "Test message!", 0xa, 0
