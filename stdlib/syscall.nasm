bits 16

global _interrupt

print_str:
	mov ah, 0Eh
.top:
	mov al, [si]
	cmp al, 0
	je .print_done

	int 10h
	inc si
	jmp .top
.print_done:
	ret

global _biosprint
_biosprint:
        push si
        mov si, debug
        call print_str
        pop si
	ret

%macro interrupt_func 1
global _interrupt_%1
_interrupt_%1:
	push bp
	mov bp,sp
	
	mov ax,[bp+4]	; load address of AX variable into ax register
	mov bx,[bp+6]	;get the other parameters BX, CX, and DX
	mov cx,[bp+8]
	mov dx,[bp+10]

        int 0x%1	;call the interrupt (00 will be changed above) => 5000:19f1
	
	pop bp
	ret
%endmacro

interrupt_func 10
interrupt_func 21

section .data

debug: db `DEBUG\r\n`, 0
intrmsg: db `INTERRUPT\r\n`, 0
