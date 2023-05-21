bits 16

global interrupt

;int interrupt (int number, int AX, int BX, int CX, int DX)
interrupt:
	push bp
	mov bp,sp
	mov ax,[bp+4]	;get the interrupt number in AL
	push ds		;use self-modifying code to call the right interrupt
	mov bx,cs
	mov ds,bx
	mov si,intr
	mov [si+1],al	;change the 00 below to the contents of AL
	pop ds
	mov ax,[bp+6]	; load address of AX variable into ax register
	mov bx,[bp+8]	;get the other parameters BX, CX, and DX
	mov cx,[bp+10]
	mov dx,[bp+12]

intr:
	int 0x00	;call the interrupt (00 will be changed above)

	pop bp
	ret

