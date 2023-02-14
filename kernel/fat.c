int read_int_num = 0x02;
int drive = 0x00; // Drive A:

//void read_chs(int cyl(20), int head(16), int sect (12), int num_sect(8), int* buf(4))
#asm
_read_chs:
    PROLOG
    
    mov ah, [_read_int_num] ; command
    
    mov dl, [_drive]
    
    mov ch, 20[bp] ; cylinder
    mov dh, 16[bp] ; head
    mov cl, 12[bp] ; sector
    mov al, 8[bp] ; num sectors
    
    mov bx, 4[bp] ; destination
    
    int 0x13
    
    EPILOG
    
    ret
#endasm


void fat_test() {
    char buf[512];
    int i;
    
    read_chs(0, 0, 0x01, 1, buf);
    
    print_string("Data:\r\n");
    for(i=0 ; i<256 ; i++){
        print_hex(buf[i]);
        print_char(' ');
    }
}
