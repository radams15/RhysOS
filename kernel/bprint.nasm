bprint:
    pusha
    mov ah, 0x0e

    bprint_loop:
        cmp byte [bx], 0
        je bprint_end

        mov al, [bx]
        int 0x10

        add bx, 1
        jmp bprint_loop

    bprint_end:
        popa
        ret
