bits 16

global _makeInterrupt21
extern _seg_copy
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

global _call_far
_call_far:
	push bp
	mov bp, sp
	
	mov ax, [bp+4]
	mov [argc], ax
	mov ax, [bp+6]
	mov [argv], ax
	mov ax, [bp+8]
	mov [stdin], ax
	mov ax, [bp+10]
	mov [stdout], ax
	mov ax, [bp+12]
	mov [stderr], ax
	
    mov ax, ss
    mov [stackseg], ax
    
    mov bx, [bp+14] ; bx => segment
    
    mov ax, [bp+16] ; ax => new ds
    mov ss, ax
    
	push bp ; new program stack frame
	mov bp, sp
	
	push WORD [stderr]
    push WORD [stdout]
    push WORD [stdin]
    push WORD [argc]
    push WORD [argv]
    mov ds, ax
        
	push cs
	push .ret
	push bx
	push 0x1008
	
	retf ; call function
.ret:
	add sp, 10 ; pop all of the program args
	pop bp ; restore stack frame for call_far
	
        mov ax, DATA_SEGMENT ; restore kernel data segment
        mov ds, ax
	
        mov ax, [stackseg] ; restore kernel stack
        mov ss, ax

        pop bp
        ret

align 16
call_addr: dw 0
call_cs: db 0
stackseg: dw 0
sp_bak: dw 0
bp_bak: dw 0

stdin: dw 0
stdout: dw 0
stderr: dw 0
argc: dw 0
argv: dw 0
