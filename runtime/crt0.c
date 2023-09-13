#define KERNEL_DATA 0x3000

extern int main(int argc, char** argv);

extern int seg_copy(char* src, char* dst, int len, int src_seg, int dst_seg);

extern int stdout;

char argv[64][64];

int start(int argc, char** argv_ext) {
    char** argv_in;

    seg_copy(argv_ext, argv_in, argc*sizeof(char*), KERNEL_DATA, ds());
    
    for(int i=0 ; i<argc ; i++) {
        seg_copy(argv_in[i], argv[i], 64, KERNEL_DATA, ds());
    }
    
    printf("Argv: %x\n", argv);
    for(int i=0 ; i<argc ; i++) {
        printf("%s, ", argv[i]);
    }

    int ret = main(argc, argv);
    
    return ret;
}
