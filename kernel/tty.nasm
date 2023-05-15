global _get_cursor
global _get_cursor_row
global _get_cursor_col

_get_cursor:
	xor ax, ax
	mov ah, 0x03
	xor bh, bh ; display page = 0
	int 0x10
	mov ax, dx
	ret
	
_get_cursor_row:
	call _get_cursor
	xor al, al
	mov al, ah
	ret

_get_cursor_col:
	call _get_cursor
	xor ah, ah
	ret
