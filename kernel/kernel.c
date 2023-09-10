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
#include "graphics.h"
#include "serial.h"

#define EXE_SIZE 8192
#define SHELL_SIZE EXE_SIZE

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

typedef struct SyscallArgs {
    int num;  // Syscall number
    int a, b, c, d, e, f;
    int cs, ds;  // Data segment
} SyscallArgs_t;

int handleInterrupt21(int* ax, int bx, int cx, int* dx) {
    SyscallArgs_t* args = ax;

    switch (args->num) {
        case 1:
            args->num =
                exec(args->a, args->b, args->c, args->d, args->e, args->f);
            break;

        case 2:
            args->num = list_directory(args->a, args->b, args->c);
            break;

        case 3:
            args->num = read(args->a, args->b, args->c);
            break;

        case 4:
            args->num = write(args->a, args->b, args->c);
            break;

        case 5:
            args->num = open(args->a);
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
            args->num = -1;
            break;
    }
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
    print_string("\n");
    exec("shell", 0, NULL, stdin, stdout, stderr);

    close(stdin);
    close(stdout);
    close(stderr);

    print_string("\n\nDone.");

    return 0;
}
