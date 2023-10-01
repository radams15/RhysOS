bits 16

section .text

global _highmem
global _lowmem

_highmem:
    mov ax, [__highmem]
    ret
    
_lowmem:
    mov ax, [__lowmem]
    ret

section .data

global __highmem;
global __lowmem;

__highmem: dw 0
__lowmem: dw 0
