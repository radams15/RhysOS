org 0x0

KSEG	equ	KERNEL_ADDR
KSIZE	equ	15
KSTART	equ	3

mov ax,KSEG
mov ds,ax
mov ss,ax
mov es,ax
mov ax,0xfff0 ; stack offset
mov sp,ax
mov bp,ax


mov     cl,KSTART+1      ;cl holds sector number
mov     dh,0     ;dh holds head number - 0
mov     ch,0     ;ch holds track number - 0
mov     ah,2            ;absolute disk read
mov     al,KSIZE        ;read KSIZE sectors
mov     dl,0            ;read from floppy disk A
mov     bx,0		;read into 0 (in the segment)
int     13h	;call BIOS disk read function

jmp KSEG:0

times 510-($-$$) db 0

; bootloader magic number
dw 0xAA55
