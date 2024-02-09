bits 16

global _sti
global _cli

extern stackseg
extern ssp
extern sbp


extern _handle_interrupt
global _init_interrupts

%macro INT_HANDLER_DECL 1
	mov si, %1
	mov dx, handle_%1
	mov [si+2],ax
	mov [si],dx	;set up our vector
%endmacro

%macro INT_HANDLER_DEFN 1
handle_%1:
    cli

	push dx
	push cx
	push bx
	push ax
	push ds
	
	mov ax, 0x3000 ; Restore KSEG
	mov ds, ax

	push %1
	
	jmp ivt_handle_end
%endmacro

INT_HANDLER_DEFN 0x00
INT_HANDLER_DEFN 0x06
INT_HANDLER_DEFN 0x70
INT_HANDLER_DEFN 0x6c
ivt_handle_end:
	call _handle_interrupt
	
	pop bx

	pop ds
	pop ax
	pop bx
	pop cx
	pop dx
	sti

	iret

; int init_interrupts()
_init_interrupts:    
	push ds
	mov ax, 0	;interrupts are in lowest memory
	mov ds,ax
	mov ax,cs	;have interrupt go to the current segment
	
    INT_HANDLER_DECL 0x00
    INT_HANDLER_DECL 0x06
    INT_HANDLER_DECL 0x70
    INT_HANDLER_DECL 0x6c
	
	pop ds
	
	mov ax, 0
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
	mov si, 0x21 * 4
	mov ax,cs	;have interrupt go to the current segment
	mov [si+2],ax
	mov [si],dx	;set up our vector
	pop ds
	pop si
	pop ax
	pop dx
	
	mov ax, 0
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

    ; restore kernel sp, bp - TODO: store program sp, bp
    mov ax, [sbp]
    mov bp, ax
    mov ax, [ssp]
    mov sp, ax

	push cx ; push ss in cx
	push ax ; push ax as argument for function
	
	call _handleInterrupt21

    ; TODO: restore program sp, bp

	pop ax
	pop ss ; restore program stack from kernel stack

	pop ax ; restore program registers from program stack
	pop bx
	pop cx
	pop dx
	pop ds

	sti

	iret

%macro INTERRUPT 1
global _interrupt_%1
;int interrupt_%1 (int AX, int BX, int CX, int DX)
_interrupt_%1:
       push bp
       mov bp,sp

       mov ax,[bp+4]   ;get the other parameters AX, BX, CX, and DX
       mov bx,[bp+6]
       mov cx,[bp+8]
       mov dx,[bp+10]

       int %1+6        ;call the interrupt (00 will be changed above)

       mov ah,0        ;we only want AL returned
       pop bp
       ret
%endmacro

INTERRUPT 10
INTERRUPT 13
INTERRUPT 14
INTERRUPT 15
INTERRUPT 16
