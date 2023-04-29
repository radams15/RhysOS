extern _main

jmp _main

extern _cls

_main
	push bp

	call _cls

	pop bp

	ret
