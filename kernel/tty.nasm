bits 16

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

global _interrupt
_interrupt:
	push bp
	mov bp,sp
	mov ax,[bp+4]	;get the interrupt number in AL
	push ds		;use self-modifying code to call the right interrupt
	mov bx,cs
	mov ds,bx
	mov si,intr
	mov [si+1],al	;change the 00 below to the contents of AL
	pop ds
	mov ax,[bp+6]	;get the other parameters AX, BX, CX, and DX
	mov bx,[bp+8]
	mov cx,[bp+10]
	mov dx,[bp+12]
intr:
	int 0x00	;call the interrupt (00 will be changed above)

	mov ah,0	;we only want AL returned
	pop bp
	ret

%macro INTERRUPT 1
global _interrupt_%1
;int interrupt_%1 (int AX, int BX, int CX, int DX)
_interrupt_%1:
        xchg bx, bx
       push bp
       mov bp,sp

       mov ax,[bp+4]   ;get the other parameters AX, BX, CX, and DX
       mov bx,[bp+6]
       mov cx,[bp+8]
       mov dx,[bp+10]

       int 0x%1        ;call the interrupt (00 will be changed above)

       mov ah,0        ;we only want AL returned
       pop bp
       ret
%endmacro

;INTERRUPT 10
;INTERRUPT 13
;INTERRUPT 14
;INTERRUPT 15
;INTERRUPT 16
