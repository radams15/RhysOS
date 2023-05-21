global lowmem_
lowmem_:
	clc
	int 12h
	ret

global highmem_
highmem_:
	clc
	mov ah, 88h
	int 15h
	ret
