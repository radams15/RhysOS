bits 16
org 7c00h

BOOT2_SEG	equ 0x0050
BOOT2_ADDR	equ	0 ; address to place kernel in
BOOT2_SIZ	equ	3 ; sectors in kernel
BOOT2_SECT	equ	2 ; start sector in initrd
SECT_PER_TRACK equ 18

jmp short boot
nop

OEMLabel		db "RHYSOS  "	; Disk label
BytesPerSector		dw 512		; Bytes per sector
SectorsPerCluster	db 1		; Sectors per cluster
ReservedForBoot		dw 1		; Reserved sectors for boot record
NumberOfFats		db 2		; Number of copies of the FAT
RootDirEntries		dw 224		; Number of entries in root dir
					; (224 * 32 = 7168 = 14 sectors to read)
LogicalSectors		dw 2880		; Number of logical sectors
MediumByte		db 0F0h		; Medium descriptor byte
SectorsPerFat		dw 9		; Sectors per FAT
SectorsPerTrack		dw 18		; Sectors per track (36/cylinder)
Sides			dw 2		; Number of sides/heads
HiddenSectors		dd 0		; Number of hidden sectors
LargeSectors		dd 0		; Number of LBA sectors
DriveNo			dw 0		; Drive No: 0
Signature		db 41		; Drive signature: 41 for floppy
VolumeID		dd 00000000h	; Volume ID: any number
VolumeLabel		db "BOOTLOAD   "; Volume Label: any 11 chars
FileSystem		db "FAT12   "	; File system type: don't change!

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
	mov ax, BOOT2_SEG ; setup segmentation
	mov ds, ax
	mov ss, ax
	mov es, ax

	mov ax, STACK_SEGMENT ; setup stack
	mov sp, ax
	mov bp, ax
	
	mov ax, 01h ; Set 40 cols mono - minimum requirements to run
	int 10h

	print boot_msg

	; Read 18 sectors of head 0


	mov     cl,BOOT2_SECT+1      ;cl holds sector number, +1 as 1-indexed
	mov     dh,0     ;dh holds head number - 0
	mov     ch,0     ;ch holds track number - 0
	mov     al, BOOT2_SIZ        ;read up to 18 sectors
	mov     bx, BOOT2_ADDR		;read into 0 (in the segment)
	mov     dl,0            ;read from floppy disk A
	mov     ah,2            ;absolute disk read
	int     13h	;call BIOS disk read function

	jc .err
.done:
	print kernel_read_msg

    jmp BOOT2_SEG:BOOT2_ADDR
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
