extern _main

%define MODE 2 ; 80x24 mode

jmp _main

extern _cls

_main:
	push bp

	lea ax, syscall_num
	mov bx, MODE
	int 21h

	pop bp

	ret

syscall_num: db 04h ; system_set_graphics_mode
