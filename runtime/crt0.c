extern int main(int argc, char** argv);

extern int seg_copy(char* src, char* dst, int len, int src_seg, int dst_seg);

void memmgr_init();

extern int stdin, stdout, stderr;

const int argv_item_size = 128;

extern void exit(unsigned char ret);

extern void kfree(void* ptr);

void start(int argc, char** argv_ext, int should_free) {
    char** argv_in;

    int my_ds = ds();

    memmgr_init();

    char** argv = 0;
    if (argc != 0) {
        argv = malloc(argc * sizeof(char));
        seg_copy(argv_ext, argv_in, argc * sizeof(char*), KERNEL_DATA_SEGMENT,
                 my_ds);

        for (int i = 0; i < argc; i++) {
            argv[i] = malloc(argv_item_size * sizeof(char));
            seg_copy(argv_in[i], argv[i], argv_item_size, KERNEL_DATA_SEGMENT,
                     my_ds);
            if (should_free)
                kfree(argv_in[i]);
        }

        if (should_free)
            kfree(argv_ext);
    }

    int ret = main(argc, argv);

    if (argc != 0) {
        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }
        free(argv);
    }

    exit(ret);
}
