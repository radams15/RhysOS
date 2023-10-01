bits 16

global _interrupt
global _read_sector
global _call_kernel
global _printc
global _ds

_printc:
	push bp
	mov bp,sp

        mov al, [bp+4]
        mov ah, 0Eh
        
        cmp al, 0Ah ; if print '\n', the print '\r\n' 
        jne .print
        
        mov al, 0Dh
        int 10h
        mov al, 0Ah
        
.print:
        int 10h
        
        pop bp
        ret
        
_ds:
    mov ax, ds
    ret

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

; void call_kernel(int ds, union SystemInfo* info)
_call_kernel:
	push bp
	mov bp, sp
	
	mov cx, [bp+6]
	mov dx, [bp+4]
	
        mov ax, DATA_SEGMENT
        mov ds, ax
        mov ss, ax
        mov es, ax
        
        push 00 ; ??
        push cx
        push dx
        push 00 ; ??
        
        jmp KERNEL_SEGMENT:0x1000
        
        pop bp
        ret
