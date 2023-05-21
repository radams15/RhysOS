bits 16

global div
div:
    PUSH BP         ; save the base pointer
    MOV BP, SP      ; set up the stack frame
    
    mov ax, word [bp+4]     ; Load dividend (num) into AX
    mov bx, word [bp+6]     ; Load divisor (base) into BX

    XOR DX, DX      ; clear the high word of the dividend
    DIV BX          ; divide the dividend by the divisor
    MOV AX, DX      ; move the quotient into AX

    POP BP          ; restore the base pointer
    RET             ; return to the calling function


global mod
mod:
    PUSH BP         ; save the base pointer
    MOV BP, SP      ; set up the stack frame
    
    mov ax, word [bp+4]     ; Load dividend (num) into AX
    mov bx, word [bp+6]     ; Load divisor (base) into BX

    XOR DX, DX      ; clear the high word of the dividend
    DIV BX          ; divide the dividend by the divisor
    MOV AX, DX      ; move the remainder into AX

    POP BP          ; restore the base pointer
    RET             ; return to the calling function


global imod
imod:
    push bp                 ; Save old base pointer
    mov bp, sp              ; Set up new base pointer
    mov ax, word [bp+4]     ; Load dividend (num) into AX
    mov dx, 0               ; Clear DX for division
    mov bx, word [bp+6]     ; Load divisor (base) into BX
    div bx                  ; Divide AX by BX (quotient in AX, remainder in DX)
    mov ax, dx              ; Move remainder from DX to AX
    pop bp                  ; Restore old base pointer
    ret                     ; Return with result in AX

