bits 16

extern _seg_copy

global _call_far
global stackseg

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
	mov ax, [bp+14]
	mov [should_free], ax
	
    mov ax, ss
    mov [stackseg], ax
    
    mov bx, [bp+16] ; bx => segment
    
    mov ax, [bp+18] ; ax => new ds
    mov ss, ax
    
	push bp ; new program stack frame
	mov bp, sp
	
	push WORD [should_free]
	push WORD [stderr]
    push WORD [stdout]
    push WORD [stdin]
    push WORD [argc]
    push WORD [argv]
    mov ds, ax
    
	push cs ; segment to return to
	push .ret ; address to return to
	push bx ; segment to jump to
    push 0x1000 ; address to jump to
	
	retf ; call function
.ret:
	add sp, 12 ; pop all of the program args
	pop bp ; restore stack frame for call_far
	
    mov ax, KERNEL_SEGMENT ; restore kernel data segment
    mov ds, ax

    mov ax, [stackseg] ; restore kernel stack
    mov ss, ax

    pop bp
    ret

stackseg: dw 0
should_free: dw 0
stdin: dw 0
stdout: dw 0
stderr: dw 0
argc: dw 0
argv: dw 0
