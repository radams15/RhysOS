global _get_cursor

_get_cursor:
	push bx
	push dx
	xor ax, ax
	mov ah, 03h
	xor bh, bh ; display page = 0
	int 10h
	mov ax, dx
	pop dx
	pop bx
	ret
