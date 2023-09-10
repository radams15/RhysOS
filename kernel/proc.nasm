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

;void handleInterrupt21 (int AX, int BX, int CX, int DX)
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

global _call_far
_call_far:
	push bp
	mov bp, sp
	
	xchg bx, bx
        mov ax, ss
        mov [stackseg], ax
        
        mov bx, [bp+14] ; bx => segment
        
        mov ax, [bp+16] ; ax => new ds
        mov ds, ax
        mov ss, ax
        
	push cs
	push .ret
	push bx
	push 0x1008
	
	retf ; call function
        
.ret:
	xchg bx, bx

        mov ax, DATA_SEGMENT
        mov ds, ax
	
        mov ax, [stackseg]
        mov ss, ax

        pop bp
        ret

align 16
call_addr: dw 0
call_cs: db 0
stackseg: dw 0
