bits 16

global _vga_disable_cursor
global _get_cursor
global _vga_setc
global _vga_scroll

_vga_disable_cursor:
    push ax
    push dx

    mov dx, 0x3D4
	mov al, 0xA	; low cursor shape register
	out dx, al

	inc dx
	mov al, 0x20	; bits 6-7 unused, bit 5 disables the cursor, bits 0-4 control the cursor shape
	out dx, al

    pop dx
    pop ax
    ret

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

_vga_scroll:
    push bp
    mov bp, sp

    push ds
    push es
    mov ax, 0b000h
    mov ds, ax ; ds => b000h
    mov es, ax ; es => b000h

    mov si, 08000h + 160
    mov di, 08000h

    ; Move data from second line to first line
    mov cx, 80*24*2    ; Set loop counter to the length of one line
    cld
    rep movsb      ; Move a block of data from ds:si to es:di

    ; Clear the last line
    ;xor ax, ax     ; Clear ax
    ;mov di, 24*80*2  ; Destination index = 24th line in VGA buffer
    ;mov cx, 160    ; Set loop counter to the length of one line
    ;rep stosw      ; Store word (2 bytes) of ax in es:di and increment di

    pop es
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
