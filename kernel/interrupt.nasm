bits 16

global _interrupt
global _sti
global _cli
global _make_rtc_interrupt
global _make_break_interrupt

extern _ctrl_break
extern stackseg

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
	
_break_handler:
	cli
	push dx
	push cx
	push bx
	push ax
	push ds
	
	mov ax, 0x3000 ; Restore KSEG
	mov ds, ax
	
	call _ctrl_break
	
	pop ds
	pop ax
	pop bx
	pop cx
	pop dx
	sti

	iret
	
_make_break_interrupt:
	mov dx,_break_handler
	push ds
	mov ax, 0	;interrupts are in lowest memory
	mov ds,ax
	mov si,0x1b*4
	mov ax,cs	;have interrupt go to the current segment
	mov [si+2],ax
	mov [si],dx	;set up our vector
	pop ds
	
	ret	

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
	cli
	
	push ds ; push regs into program stack
	push dx
	push cx
	push bx
	push ax
	
	mov cx, ss ; program ss in cx
	
        mov bx, DATA_SEGMENT ; restore kernel ds, ss
        mov ds, bx
        mov bx, [stackseg]
        mov ss, bx
	
	push cx ; push ss in cx
	push ax ; push ax as argument for function
	
	call _handleInterrupt21
	
	pop ax
	pop ss ; restore program stack from kernel stack

	pop ax ; restore program registers from program stack
	pop bx
	pop cx
	pop dx
	pop ds
	
	sti

	iret

call_addr: dw 0
call_cs: db 0
sp_bak: dw 0
bp_bak: dw 0
