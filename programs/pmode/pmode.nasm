bits 16

global _main

extern _write

_main:
   mov bx, 0f01h         ; attrib/char of smiley
   mov eax, 0b8000h      ; note 32 bit offset
   mov word [ds:eax], bx

   push 5
   push hello
   push 1
   call _write

   ret

section .data

hello: db 'hello'
