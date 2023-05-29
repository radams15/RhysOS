bits 16
org 7c00h


KSEG	equ	KERNEL_ADDR ; address to place kernel in
KSIZE	equ	KERNEL_SECTORS ; sectors in kernel
KSTART	equ	2 ; start sector in initrd

jmp 0:boot

%macro print 1 ; destroys si, ax, dx
	push si
	push ax
	push ds
	
	mov ax, 0
	mov ds, ax
	
	mov si, boot_msg
	call print_str
	
	pop ds
	pop ax
	pop si
%endmacro

print_str:
	mov ah, 0Eh
.top
	mov al, [si]
	cmp al, 0
	je .print_done

	int 10h
	inc si
	jmp .top
.print_done:
	ret

boot:
	mov ax, KSEG ; setup segmentation
	mov ds, ax
	mov ss, ax
	mov es, ax

	mov ax, STACK_ADDR ; setup stack
	mov sp, ax
	mov bp, ax
	
	print boot_msg
	
	mov     cl,KSTART+1      ;cl holds sector number
	mov     dh,0     ;dh holds head number - 0
	mov     ch,0     ;ch holds track number - 0
	mov     ah,2            ;absolute disk read
	mov     al,KSIZE        ;read KSIZE sectors
	mov     dl,0            ;read from floppy disk A
	mov     bx,0		;read into 0 (in the segment)
	int     13h	;call BIOS disk read function

.end:
	jmp KSEG:0
	jmp .end

boot_msg: db 'Booting RhysOS...', 0xa, 0xd, 0

times 510-($-$$) db 0

; bootloader magic number
dw 0xAA55
