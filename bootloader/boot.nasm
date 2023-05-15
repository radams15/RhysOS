bits 16

section .text

KSEG	equ	KERNEL_ADDR ; address to place kernel in
KSIZE	equ	KERNEL_SECTORS ; sectors in kernel
KSTART	equ	2 ; start sector in initrd

mov si, boot_msg
mov ah, 0x0E
	
print_char:
	mov al, [si]
	cmp al, 0
	je print_done

	int 0x10
	inc si
	jmp print_char

print_done:

mov ax,KSEG
mov ds,ax
mov ss,ax
mov es,ax
mov ax,STACK_ADDR ; stack offset
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

boot_msg: db 'Booting RhysOS...', 0xa, 0xd, 0

times 510-($-$$) db 0

; bootloader magic number
dw 0xAA55
