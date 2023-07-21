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
;void handleInterrupt21 (int AX, int BX, int CX, int DX, int DS)
_interrupt21ServiceRoutine:
        push ds
	push dx
	push cx
	push bx
	push ax
	
	cli
	
	mov ax, 0x3000 ; Restore KSEG
	mov ds, ax
	
	call _handleInterrupt21
	
	sti

	pop ax
	pop bx
	pop cx
	pop dx
	pop ds

	iret

%macro far_call_func 2
global _call_%1
_call_%1:
        push ds

        call %1:%2
        
        pop ds
        ret
%endmacro

far_call_func EXE_SEGMENT, 0x1008
far_call_func SHELL_SEGMENT, 0x1008

section .data

SEG_STORE: db 0x1000
