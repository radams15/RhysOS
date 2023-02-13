floppy_load:
    ; Load kernel from floppy disk.
    ; BX: arg 1 - destination buffer
    ; DH: arg 2 - number of sectors.

    push dx

    mov ax, bx   ; setting up the address to read into
    mov es, ax       ; moving the value in to es
    xor bx, bx       ; clearing bx
    mov ah, 0x02     ; floppy function
    mov al, dh       ; read 1 sector
    mov ch, 0        ; cylinder
    mov cl, 2        ; sector to read
    mov dh, 0        ; head number
    mov dl, 0        ; drive number
    int 0x13            ; Actual reading

    jc floppy_error
    
    pop dx
    ret

floppy_error:
    mov bx, DISK_ERROR_MSG
    call bprint
    jmp $

DISK_ERROR_MSG:
    db "Floppy read error!", 0
