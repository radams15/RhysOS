bits 16

extern _main

%define MODE 2 ; 80x24 mode

jmp _main

extern _cls

_main:
	push bp

	call _cls

	pop bp

	ret
