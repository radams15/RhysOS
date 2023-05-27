bits 16

global _graphics_putc2
    
_graphics_putc2:
	push bp
	mov bp, sp
	
	mov ah, 0Ch
	
	mov bh, 0
	mov cx, [bp+4]
	mov dx, [bp+6]
	mov al, [bp+8]
	int 10h
	
	pop bp
	ret