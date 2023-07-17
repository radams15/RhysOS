bits 16

global _makeInterrupt21
extern _handleInterrupt21

_makeInterrupt21:
	;get the address of the service routine
	push dx
	push ax
	push si
	mov dx, _interrupt21ServiceRoutine ; causes a crash
	
	push ds
	mov ax, 0	;interrupts are in lowest memory
	mov ds,ax
	mov si,0x84	;interrupt 0x21 vector (21 * 4 = 84)
	mov ax,cs	;have interrupt go to the current segment
	mov [si+2],ax
	mov [si],dx	;set up our vector
	pop ds
	pop si
	pop ax
	pop dx
	ret

;this is called when interrupt 21 happens
;it will call your function:
;void handleInterrupt21 (int AX, int BX, int CX, int DX)
_interrupt21ServiceRoutine:
	push dx
	push cx
	push bx
	push ax
	sti
	call _handleInterrupt21
	pop ax
	pop bx
	pop cx
	pop dx

	iret

global _call_prog
_call_prog:
	push bp
	mov bp, sp
        
        call 0x5000:0x1008 ; requires 'retf' in process to return here
        
	pop bp
	ret

