global makeInterrupt21_
extern handleInterrupt21_

makeInterrupt21_:
	;get the address of the service routine
	mov dx,interrupt21ServiceRoutine_
	push ds
	mov ax, 0	;interrupts are in lowest memory
	mov ds,ax
	mov si,0x84	;interrupt 0x21 vector (21 * 4 = 84)
	mov ax,cs	;have interrupt go to the current segment
	mov [si+2],ax
	mov [si],dx	;set up our vector
	pop ds
	ret

;this is called when interrupt 21 happens
;it will call your function:
;void handleInterrupt21 (int AX, int BX, int CX, int DX)
interrupt21ServiceRoutine_:
	push dx
	push cx
	push bx
	push ax
	sti
	call handleInterrupt21_
	pop ax
	pop bx
	pop cx
	pop dx

	iret
