bits 16

global _read_sector_to_segment

; void read_sector_to_segment(int disk, int track, int head, int sector, int dst_addr, int dst_seg);
_read_sector_to_segment:
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
	
