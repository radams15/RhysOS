global get_cursor_
global get_cursor_row_
global get_cursor_col_

get_cursor_:
	xor ax, ax
	mov ah, 0x03
	xor bh, bh ; display page = 0
	int 0x10
	mov ax, dx
	ret
	
get_cursor_row_:
	call get_cursor_
	xor al, al
	mov al, ah
	ret

get_cursor_col_:
	call get_cursor_
	xor ah, ah
	ret
