#define KERNEL_DATA 0x3000

extern int main(int argc, char** argv);

extern int seg_copy(char* src, char* dst, int len, int src_seg, int dst_seg);

void memmgr_init();

extern int stdout;

const int argv_item_size = 128;

int start(int argc, char** argv_ext) {
    char** argv_in;
    
    memmgr_init();
    
    char** argv = malloc(argc * sizeof(char));
    printf("Argv: %x\n", argv);

    seg_copy(argv_ext, argv_in, argc*sizeof(char*), KERNEL_DATA, ds());
    
    for(int i=0 ; i<argc ; i++) {
        argv[i] = malloc(argv_item_size * sizeof(char));
        seg_copy(argv_in[i], argv[i], argv_item_size, KERNEL_DATA, ds());
    }
    
    for(int i=0 ; i<argc ; i++) {
        printf("%s, ", argv[i]);
    }

    int ret = main(argc, argv);
    
    return ret;
}
