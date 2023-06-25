global _get_cursor
global _get_cursor_row
global _get_cursor_col

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
	
_get_cursor_row:
	;call _get_cursor
	xor al, al
	mov al, ah
	ret

_get_cursor_col:
	;call _get_cursor
	xor ah, ah
	ret