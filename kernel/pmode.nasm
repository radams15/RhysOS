bits 16

global _call_pmode

_call_pmode:
   cli                    ; no interrupts
   push ds                ; save real mode

   lgdt [gdtinfo]         ; load gdt register

   mov  eax, cr0          ; switch to pmode by
   or al,1                ; set pmode bit
   mov  cr0, eax
   xchg bx, bx
   jmp 0x8:0x1000

gdtinfo:
   dw gdt_end - gdt - 1   ;last byte in table
   dd gdt                 ;start of table

gdt:        dd 0,0        ; entry 0 is always unused
codedesc:   db 0xff, 0xff, 0, 0, 0, 10011010b, 00000000b, 0
flatdesc:   db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0
gdt_end:
