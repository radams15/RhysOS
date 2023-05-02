global _lowmem
_lowmem:
	clc
	int 12h
	ret

global _highmem
_highmem:
	clc
	mov ah, 88h
	int 15h
	ret
