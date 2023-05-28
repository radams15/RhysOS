global _div
_div:
    PUSH BP         ; save the base pointer
    MOV BP, SP      ; set up the stack frame
    
    mov ax, word [bp+4]     ; Load dividend (num) into AX
    mov bx, word [bp+6]     ; Load divisor (base) into BX

    XOR DX, DX      ; clear the high word of the dividend
    DIV BX          ; divide the dividend by the divisor
    MOV AX, DX      ; move the quotient into AX

    POP BP          ; restore the base pointer
    RET             ; return to the calling function


global _mod
_mod:
    PUSH BP         ; save the base pointer
    MOV BP, SP      ; set up the stack frame
    
    mov ax, word [bp+4]     ; Load dividend (num) into AX
    mov bx, word [bp+6]     ; Load divisor (base) into BX

    XOR DX, DX      ; clear the high word of the dividend
    DIV BX          ; divide the dividend by the divisor
    MOV AX, DX      ; move the remainder into AX

    POP BP          ; restore the base pointer
    RET             ; return to the calling function


global _imod
_imod:
    push bp                 ; Save old base pointer
    mov bp, sp              ; Set up new base pointer
    mov ax, word [bp+4]     ; Load dividend (num) into AX
    mov dx, 0               ; Clear DX for division
    mov bx, word [bp+6]     ; Load divisor (base) into BX
    div bx                  ; Divide AX by BX (quotient in AX, remainder in DX)
    mov ax, dx              ; Move remainder from DX to AX
    pop bp                  ; Restore old base pointer
    ret                     ; Return with result in AX

global _outb
_outb:
	push bp
	mov bp, sp
	
	push dx
	
	mov dx, [bp+4] ; port
	mov al, [bp+6] ; data
	out dx, al
	
	pop bx
	
	pop bp
	ret
	
global _inb
_inb:
	push bp
	mov bp, sp
	
	push dx
	
	mov dx, [bp+4] ; port
	in ax, dx
	
	pop bx
	
	pop bp
	ret
