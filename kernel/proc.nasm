bits 16

extern _seg_copy

global _call_far
global stackseg
global ssp

_call_far:
    cli
    xchg bx, bx
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
    
    mov ax, sp
    mov [ssp], ax
    mov ax, 0ff00h ; new stack @ ff00h
    mov sp, ax

    mov bx, [bp+16] ; bx => segment
    
    mov ax, [bp+18] ; ax => new ds, ss
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
    
	push cs
	push .ret
	push bx
	push 0x1000
	
	retf ; call function
.ret:
	add sp, 12 ; pop all of the program args
	pop bp ; restore stack frame for call_far
	
    mov ax, DATA_SEGMENT ; restore kernel data segment
    mov ds, ax

    mov ax, [ssp] ; restore sp
    mov sp, ax

    mov ax, [stackseg] ; restore kernel stack
    mov ss, ax

    pop bp
    xchg bx, bx
    sti
    ret

stackseg: dw 0
ssp: dw 0
should_free: dw 0
stdin: dw 0
stdout: dw 0
stderr: dw 0
argc: dw 0
argv: dw 0
