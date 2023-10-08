#define KERNEL_DATA 0x3000

extern int main(int argc, char** argv);

extern int seg_copy(char* src, char* dst, int len, int src_seg, int dst_seg);

void memmgr_init();

extern int stdout;

const int argv_item_size = 128;

int start(int argc, char** argv_ext) {
    char** argv_in;
    
    int my_ds = ds();
    
    memmgr_init();
    
    char** argv = 0;
    if(argc != 0) {
        argv = malloc(argc * sizeof(char));
        seg_copy(argv_ext, argv_in, argc*sizeof(char*), KERNEL_DATA, my_ds);
        
        for(int i=0 ; i<argc ; i++) {
            argv[i] = malloc(argv_item_size * sizeof(char));
            seg_copy(argv_in[i], argv[i], argv_item_size, KERNEL_DATA, my_ds);
        }
    }
    
    int ret = main(argc, argv);
    
    return ret;
}
