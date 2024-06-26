bits 16

global _main

_main:
   jmp short pmode

gdtinfo:
gdt_len:   dw gdt_end - gdt - 1   ;last byte in table
   dd gdt                 ;start of table

gdt:        dd 0,0        ; entry 0 is always unused
codedesc:   db 0xff, 0xff, 0, 0, 0, 10011010b, 00000000b, 0
flatdesc:   db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0
gdt_end:

pmode:
   mov  bx, 0x10          ; select descriptor 2, instead of 1
   mov  ds, bx            ; 10h = 10000b

   mov bx, 0x0f01         ; attrib/char of smiley
   mov eax, 0x0b8000      ; note 32 bit offset
   mov word [ds:eax], bx

   and al,0xFE            ; back to realmode
   mov  cr0, eax          ; by toggling bit again

   xchg bx, bx

   retf
