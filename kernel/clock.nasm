bits 16

global _clock_ticks
global _clock_time

; unsigned int clock_ticks(unsigned int* buf);
_clock_ticks:
    push bp
    mov bp, sp
    
    push dx
    push cx
    push si
    
    mov ah, 0
    int 0x1a
    
    mov si, [bp+4]
    
    mov [si], dx   ; low byte
    mov [si+2], cx ; high byte
    
    pop si
    pop cx
    pop dx
    
    pop bp
    
    xor ah, ah
    
    ret

; int clock_time(unsigned int* buf);
_clock_time:
    push bp
    mov bp, sp
    
    push dx
    push cx
    push si
    
    mov ah, 2
    int 0x1a
    
    jnc .noerr
    
    mov ax, 1
    jmp .end
    
.noerr:
    mov ax, 0
    
    mov si, [bp+4]
    
    mov [si], ch ; hours
    mov [si+2], cl ; minutes
    mov [si+4], dh ; seconds
    mov [si+6], dl ; daylight savings

.end:
    pop si
    pop cx
    pop dx
    
    pop bp
    
    ret
