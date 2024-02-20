bits 16

global _get_cursor
global _vga_setc

_vga_setc:
    push bp
    mov bp, sp

    push ds
    push si
    push bx
    push cx

    mov ax, 0b000h
    mov ds, ax ; ds => b000h

    mov bx, 08000h ; vga buffer
    mov si, [bp+4] ; int pos
    mov ax, [bp+6] ; char c
    mov cx, [bp+8] ; char colour c


    shl si, 1 ; si *= 2 for int index rather than char
    mov [bx+si], al ; char
    mov [bx+si+1], cl ; char colour

    pop cx
    pop bx
    pop si
    pop ds

    pop bp
    ret

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
