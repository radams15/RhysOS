bits 16

global _interrupt
global _sti
global _cli
global _make_rtc_interrupt

;int interrupt (int number, int AX, int BX, int CX, int DX)
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


extern _tick

_tick_handler:
	cli
	push dx
	push cx
	push bx
	push ax
	push ds
	
	mov ax, 0x3000 ; Restore KSEG
	mov ds, ax
	
	call _tick
	
	pop ds
	pop ax
	pop bx
	pop cx
	pop dx
	sti

	iret

_make_rtc_interrupt:
	mov dx,_tick_handler
	push ds
	mov ax, 0	;interrupts are in lowest memory
	mov ds,ax
	mov si,0x1c*4
	mov ax,cs	;have interrupt go to the current segment
	mov [si+2],ax
	mov [si],dx	;set up our vector
	pop ds
	
	ret

_sti:
	sti
	ret

_cli:
	cli
	ret
