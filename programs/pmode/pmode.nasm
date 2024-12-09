bits 32

global _main

_main:
   mov bx, 0f01h         ; attrib/char of smiley
   mov eax, 0b8000h      ; note 32 bit offset
   mov word [ds:eax], bx

   mov ax, 1h
   int 21h

   ret
