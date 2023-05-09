bits 16

global lowmem
lowmem:
	clc
	int 12h
	ret

global highmem
highmem:
	clc
	mov ah, 88h
	int 15h
	ret
