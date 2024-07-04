bits 16

global _main

_main:
   mov bx, 0x0f01         ; attrib/char of smiley
   mov eax, 0x0b8000      ; note 32 bit offset
   mov word [ds:eax], bx

   ret
