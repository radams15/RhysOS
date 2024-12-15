bits 32

global _interrupt
global _ds
global _cs
global _ss

_ds:
        mov ax, ds
        ret
_cs:
	mov ax, cs
	ret
	
_ss:
	mov ax, ss
	ret


%macro interrupt_func 1
global _interrupt_%1
_interrupt_%1:
	push bp
	mov bp,sp
	
	push ds
	
	mov ax,[bp+4]	; load address of AX variable into ax register
	mov bx,[bp+6]	;get the other parameters BX, CX, and DX
	mov cx,[bp+8]
	mov dx,[bp+10]

    int 0x%1
        
    pop ds
	
	pop bp
	ret
%endmacro

interrupt_func 10
interrupt_func 21
