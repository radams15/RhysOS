bits 16

KSTART	equ	2 ; start sector in initrd

jmp boot

%macro print 1 ; destroys si, ax
	push si
	push ax
	
	mov si, %1
	mov ah, 0Eh
	call print_str
	
	pop ax
	pop si
%endmacro

print_str:
	mov al, [si]
	cmp al, 0
	je .print_done

	int 10h
	inc si
	jmp print_str
.print_done:
	ret
	
drive_reset:
	xor ah, ah
	mov dl, 0 ; A: drive
	int 13h
	jc drive_reset ; fail? try again
	
	ret

boot:
	;call drive_reset

	mov ax,KERNEL_ADDR
	mov ds,ax
	mov ss,ax
	mov es,ax
	mov ax,STACK_ADDR ; stack offset
	mov sp,ax
	mov bp,ax
	
	print boot_msg

	mov     cl,KSTART+1      ;cl holds sector number
	mov     dh,0     ;dh holds head number - 0
	mov     ch,0     ;ch holds track number - 0
	mov     ah,2            ;absolute disk read
	mov     al,KERNEL_SECTORS        ;read KSIZE sectors
	mov     dl,0            ;read from floppy disk A
	mov     bx,0		;read into 0 (in the segment)
	
	;int     13h	;call BIOS disk read function
	
	jc .disk_fail
	
	print read_success_msg
	
	;jmp KERNEL_ADDR:0

.disk_fail:
	print read_fail_msg
	jmp .disk_fail

boot_msg: db `Reading Kernel...\r\n`, 0
read_fail_msg: db `Failed to read floppy!\r\n`, 0
read_success_msg: db `Read complete, loading OS!\r\n`, 0
test_msg: db `Got here\r\n`, 0

times 510-($-$$) db 0

; bootloader magic number
dw 0AA55h
