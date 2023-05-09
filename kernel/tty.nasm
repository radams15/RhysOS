global _get_cursor

_get_cursor:
	xor ax, ax
	mov ah, 0x02
	xor bh, bh ; display page = 0
	int 0x10
	mov ax, dx
	ret
