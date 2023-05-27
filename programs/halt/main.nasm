extern _main

jmp _main

_main:
	push bp
	
	mov ax, 5307h ; apm
	mov bx, 1 ; everything
	mov cx, 3 ; shutdown
	int 15h

	pop bp

	ret
