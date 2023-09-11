void main(int rootfs_start);
void entry(int a) {
    main(a);
}

#include "fs/fs.h"
#include "malloc.h"
#include "proc.h"
#include "tty.h"

#include "fs/devfs.h"
#include "fs/fat.h"

#include "clock.h"
#include "serial.h"

#define EXE_SIZE 8192
#define SHELL_SIZE EXE_SIZE
#define SYSCALL_BUF_SIZ 64

int stdin, stdout, stderr;

int init(int rootfs_start);

void main(int rootfs_start) {
    int err;

    err = init(rootfs_start);

    if (err) {
        print_string("\r\nError in kernel, halting!\r\n");
    }

    for (;;) {
    }
}

int list_directory(char* dir_name, FsNode_t* buf, int max) {
    int i = 0;
    int count = 0;
    DirEnt_t* node = NULL;
    FsNode_t* fsnode;
    FsNode_t* root = get_dir(dir_name);

    if (root == NULL) {
        print_string("Cannot find directory!\n");
        return 0;
    }

    while ((node = fs_readdir(root, i)) != NULL && count <= max) {
        fsnode = fs_finddir(root, node->name);
        if (fsnode != NULL) {
            if (buf != NULL)
                memcpy(buf++, fsnode, sizeof(FsNode_t));
            count++;
        }

        i++;
    }

    return count;
}

void debug(const char* label, int data) {
	print_string(label);
	printi(data, 16);
	print_string("\n");
}

typedef struct SyscallArgs {
    int num;  // Syscall number
    int a, b, c, d, e, f;
    int cs, ds;  // Data segment
} SyscallArgs_t;

int seg_copy(char* src, char* dst, int len, int src_seg, int dst_seg);

int i21_handler(SyscallArgs_t* args) {
    //debug("SC: ", args->num);

    switch (args->num) {
        case 1: {
                //int execa(char* file, int argc, char** argv, int in, int out, int err) {
                char name[64];
		        seg_copy(args->a, name, sizeof(name), args->ds, DATA_SEGMENT);
                return exec(name, args->b, args->c, args->d, args->e, args->f);
            }
            break;

        case 2:
            return list_directory(args->a, args->b, args->c);
            break;

        case 3: {
                int read_len = args->c;
                char buf[SYSCALL_BUF_SIZ];
                char* dst = args->b;
                
                int ret = 0;
                
                while(read_len > SYSCALL_BUF_SIZ) {
                    read_len -= SYSCALL_BUF_SIZ;
                    
                    ret += read(args->a, &buf, SYSCALL_BUF_SIZ);
                    seg_copy(&buf, dst, SYSCALL_BUF_SIZ, DATA_SEGMENT, args->ds);
                    
                    dst += ret;
                }
                
                ret += read(args->a, &buf, read_len);
                seg_copy(&buf, dst, read_len, DATA_SEGMENT, args->ds);
                
                return ret;
            }
            break;

        case 4: {
	            char text[SYSCALL_BUF_SIZ];
		        seg_copy(args->b, text, sizeof(text), args->ds, DATA_SEGMENT);
        	    return write(args->a, text, args->c);
            }
            break;

        case 5: {
	            char name[SYSCALL_BUF_SIZ];
		        seg_copy(args->a, name, sizeof(name), args->ds, DATA_SEGMENT);
		        return open(name);
            }
            break;

        case 6:
            close(args->a);
            break;

        case 7:
            seek(args->a, args->b);
            break;

        default:
            print_string("Unknown interrupt: ");
            printi(args->a, 16);
            print_string("!\r\n");
            return -1;
            break;
    }
    
    return 0;
}

int handleInterrupt21(int* ax, int ss, int cx, int dx) { 
    SyscallArgs_t arg_data;
    seg_copy(ax, &arg_data, sizeof(SyscallArgs_t), ss, DATA_SEGMENT);
    
    arg_data.num = i21_handler(&arg_data);
    
    seg_copy(&arg_data, ax, sizeof(SyscallArgs_t), DATA_SEGMENT, ss);
}

void a20_init() {
    if (a20_available()) {
        int enable_fail;
        print_string("A20 line is available\n");
        enable_fail = a20_enable();

        if (enable_fail)
            print_string("A20 line failed to enable\n");
        else
            print_string("A20 line successfully enabled\n");
    } else {
        print_string("A20 line is unavaiable\n");
    }
}

int init(int rootfs_start) {
    cls();
    FsNode_t* fs_dev;

    a20_init();

    memmgr_init();
    print_string("Memory manager enabled\n");

    makeInterrupt21();
    print_string("Int 21h enabled\n");

    rtc_init();
    print_string("RTC enabled\n");

    serial_init(COM1, BAUD_9600, PARITY_NONE, STOPBITS_ONE, DATABITS_8);
    print_string("/dev/COM1 enabled\n");

    fs_root = fat_init(rootfs_start);
    fs_dev = devfs_init();
    fat_mount(fs_dev, "dev");
    print_string("Root filesystem mounted\n");

    stdin = open("/dev/stdin");
    stdout = open("/dev/stdout");
    stderr = open("/dev/stderr");

    print_string("Welcome to RhysOS!\n\n");
    
    //fat_create("out.txt");

    exec("mem", 0, NULL, stdin, stdout, stderr);
    print_string("\nNext\n");
    exec("shell", 0, NULL, stdin, stdout, stderr);

    close(stdin);
    close(stdout);
    close(stderr);

    print_string("\n\nDone.");

    return 0;
}
