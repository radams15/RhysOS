bits 16

global _interrupt
global _read_sector
global _call_kernel

;int interrupt (int number, int AX, int BX, int CX, int DX)
_interrupt:
	push bp
	mov bp,sp
	push ds		;use self-modifying code to call the right interrupt
	mov bx,cs
	mov ds,bx
	mov si,intr
	mov ax,[bp+4]	;get the interrupt number in AL
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

; void read_sector(int disk, int track, int head, int sector, int dst_addr, int dst_seg);
_read_sector:
	push bp
	mov bp, sp

	push es

	mov ax, [bp+14] ; dst_sector -> ax
	mov es, ax ; dst_sector -> es

	mov     cl, [bp+10]      ;cl holds sector number, +1 as 1-indexed
	mov     dh, [bp+8]     ;dh holds head number - 0
	mov     ch, [bp+6]     ;ch holds track number - 0
	mov     al, 1        ;read up to 18 sectors
	mov     bx, [bp+12]          ;read into 0 (in the segment)
	mov     dl, [bp+4]            ;read from floppy disk A
	mov     ah,2            ;absolute disk read
	int     13h     ;call BIOS disk read function

	pop es

	pop bp
	ret
	
printc:
        push bx
        mov ah, 0Eh
        xor bx, bx
        int 10h
        pop bx
        ret

; void call_kernel()
_call_kernel:
	push bp
	mov bp, sp

        push ds

        mov ax, 0x3000
        mov ds, ax
        mov ss, ax
        mov es, ax
        jmp 0x2000:0x1000
        
        pop ds

	pop bp
	ret
