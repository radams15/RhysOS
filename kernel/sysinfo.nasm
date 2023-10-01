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

__highmem: dw 0x5678
__lowmem: dw 0x1234
