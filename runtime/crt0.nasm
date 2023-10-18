bits 16

times 8 nop

jmp _crt0

global _crt0
extern _start
extern _main

extern _stdin
extern _stdout
extern _stderr

extern _printf

_crt0:
    push bp
    mov bp, sp
    
    mov dx, [bp+16] ; should_free
    mov bx, [bp+8] ; argc
    mov cx, [bp+6] ; argv

    mov al, [bp+10] ; set stdin, stdout, stderr from the stack
    mov [_stdin], al
    
    mov al, [bp+12]
    mov [_stdout], al
    
    mov al, [bp+14]
    mov [_stderr], al
    
    push DWORD dx ; should_free
    push DWORD cx ; argv
    push DWORD bx ; argc
    call _start
    add sp, 6
    
    pop bp
    
    xchg bx, bx
    
    retf

;int seg_copy(char* src, char* dst, int len, int src_seg, int dst_seg);
global _seg_copy
_seg_copy:
	push bp
	mov bp, sp
	
	push ds
	push es
	push si
	push cx
	push di
	
	mov ax, [bp+10]
	mov ds, ax ; ds => start seg
	
	mov si, [bp+4] ; si => src address
	
	mov ax, [bp+12]
	mov es, ax ; es => dest seg
	
	mov di, [bp+6] ; di => dest address
	mov cx, [bp+8] ; cx => length
	
	rep movsb
	
	pop di
	pop cx
	pop si
	pop es
	pop ds
	
	pop bp
	ret
