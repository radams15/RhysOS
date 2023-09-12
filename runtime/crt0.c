#define KERNEL_DATA 0x3000

extern int main(int argc, char** argv);

extern int seg_copy(char* src, char* dst, int len, int src_seg, int dst_seg);

extern int stdout;

int start(int argc, char** argv_ext) {
    printf("ARGC: %x\n", argc);
    printf("ARGV: %x\n", argv_ext);
    
    asm volatile("xchg %bx, %bx");
    
    char argv[64][64];

    char** argv_in;

    seg_copy(argv_ext, argv_in, argc*sizeof(char*), KERNEL_DATA, ds());
    
    for(int i=0 ; i<argc ; i++) {
        seg_copy(argv_in[i], argv[i], 64, KERNEL_DATA, ds());
        printf("%s, ", argv[i]);
    }

    int ret = main(0, 0);
    
    return ret;
}
