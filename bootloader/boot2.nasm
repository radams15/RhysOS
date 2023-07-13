bits 16

global _interrupt
global _read_sector

;int interrupt (int number, int AX, int BX, int CX, int DX)
_interrupt:
	push bp
	mov bp,sp
	mov ax,[bp+4]	;get the interrupt number in AL
	push ds		;use self-modifying code to call the right interrupt
	mov bx,cs
	mov ds,bx
	mov si,intr
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

; void read_sector(int disk, int track, int head, int sector, int dst_addr, int dst_sector);
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
