extern _main

%define MODE 2 ; 80x24 mode

jmp _main

extern _cls

_main:
	push bp

	mov ax, 04h ; system_set_graphics_mode
	mov bx, MODE
	int 21h

	pop bp

	ret
