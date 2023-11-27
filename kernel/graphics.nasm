bits 16


global _fp_putb

; int fp_putb(int es, int di, int val);
_fp_putb:
    cli
    push bp
    mov bp, sp

    mov ax, [bp+4] ; es
    mov es, ax

    mov ax, [bp+6] ; di
    mov di, ax

    mov al, [bp+8] ; val
    
    mov [es:di], al

    pop bp
    mov ax, 1
    sti
    ret
