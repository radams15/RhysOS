bits 16
org 7c00h


KSEG	equ	KERNEL_ADDR ; address to place kernel in
KSIZE	equ	KERNEL_SECTORS ; sectors in kernel
KSTART	equ	2 ; start sector in initrd
SECT_PER_TRACK equ 18

jmp 0:boot

%macro print 1 ; destroys si, ax, dx
	push si
	push ax
	push ds
	
	mov ax, 0
	mov ds, ax
	
	mov si, %1
	call print_str
	
	pop ds
	pop ax
	pop si
%endmacro

print_str:
	mov ah, 0Eh
.top:
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
	mov es, ax
	mov ss, ax

	mov ax, STACK_ADDR ; setup stack
	mov sp, ax
	mov bp, ax
	
	print boot_msg
	
	; Read 18 sectors of head 0
	
	
	mov     cl,KSTART+1      ;cl holds sector number
	mov     dh,0     ;dh holds head number - 0
	mov     ch,0     ;ch holds track number - 0
	mov     al,SECT_PER_TRACK-KSTART        ;read up to 18 sectors
	mov     bx,0		;read into 0 (in the segment)
	mov     dl,0            ;read from floppy disk A
	mov     ah,2            ;absolute disk read
	int     13h	;call BIOS disk read function
	
	jc .err
	
	; Read up to 18 more sectors on head 1
	
	add bx, ((SECT_PER_TRACK-KSTART)*512)
	
	mov     cl,1      ;cl holds sector number
	mov     dh,1     ;dh holds head number - 0
	mov     ch,0     ;ch holds track number - 0
	mov     al, (SECT_PER_TRACK)       ;read rest of sectors
	mov     dl,0            ;read from floppy disk A
	mov     ah,2            ;absolute disk read
	int     13h	;call BIOS disk read function
	
	jc .err

.done:
	print kernel_read_msg

    jmp KSEG:0
.end:
	jmp $
	
.err:
	print err_msg
	jmp $
	
boot_msg: db 'Booting RhysOS...', 0xa, 0xd, 0
kernel_read_msg: db 'Kernel read complete!', 0xa, 0xd, 0
err_msg: db 'Disk read error!', 0xa, 0xd, 0

buf times 64 db 0

times 510-($-$$) db 0

; bootloader magic number
dw 0xAA55
