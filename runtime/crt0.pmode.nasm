bits 16

global _crt0
extern _start
extern _main

extern _stdin
extern _stdout
extern _stderr

extern _printf

jmp short _crt0

gdtinfo:
   dw gdt_end - gdt - 1   ;last byte in table
   dd gdt                 ;start of table

gdt:        dd 0,0        ; entry 0 is always unused
codedesc:   db 0xff, 0xff, 0, 0, 0, 10011010b, 00000000b, 0
flatdesc:   db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0
gdt_end:

_crt0:
   xchg bx, bx
   push bp
   mov bp, sp

   mov  bx, 0x10          ; select descriptor 2, instead of 1
   mov  ds, bx            ; 10h = 10000b

   mov bx, 0x0f01         ; attrib/char of smiley
   mov eax, 0x0b8000      ; note 32 bit offset
   mov word [ds:eax], bx

   and al,0xFE            ; back to realmode
   mov  cr0, eax          ; by toggling bit again

   pop bp

   retf

;int seg_copy(char* src, char* dst, int len, int src_seg, int dst_seg);
global _seg_copy
_seg_copy:
	push bp
	mov bp, sp
	
	push ds
	push es
	push si
	push cx
	push di
	
	mov ax, [bp+10]
	mov ds, ax ; ds => start seg
	
	mov si, [bp+4] ; si => src address
	
	mov ax, [bp+12]
	mov es, ax ; es => dest seg
	
	mov di, [bp+6] ; di => dest address
	mov cx, [bp+8] ; cx => length
	
	rep movsb
	
	pop di
	pop cx
	pop si
	pop es
	pop ds
	
	pop bp
	ret
