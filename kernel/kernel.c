void main(int src_ds, void* boot_ptr);
void entry(int src_ds, void* boot_ptr) {
    main(src_ds, boot_ptr);
}

#include "fs/fs.h"
#include "malloc.h"
#include "proc.h"
#include "sysinfo.h"
#include "tty.h"
#include "util.h"

#include "fs/devfs.h"
#include "fs/fat.h"

#include "clock.h"
#include "serial.h"

#define EXE_SIZE 8192
#define SHELL_SIZE EXE_SIZE
#define SYSCALL_BUF_SIZ 1024

struct SystemInfo {
    int rootfs_start;
    int highmem;
    int lowmem;
    char cmdline[256];
};

int stdin, stdout, stderr;

int init(struct SystemInfo* info);

void main(int src_ds, void* boot_ptr) {
    int err;

    struct SystemInfo info;

    seg_copy(boot_ptr, &info, sizeof(struct SystemInfo), src_ds, DATA_SEGMENT);

    err = init(&info);

    if (err) {
        print_string("\r\nError in kernel, halting!\r\n");
    }

    for (;;) {
    }
}

void debug(const char* label, int data) {
    print_string(label);
    printi(data, 16);
    print_string("\n");
}

void kdir(char* dir_name) {
    int i = 0;
    DirEnt_t* node = NULL;
    FsNode_t* fsnode;
    FsNode_t* root = get_dir(dir_name);

    if (root == NULL) {
        print_string("Cannot find directory!\n");
        return 0;
    }

    while ((node = fs_readdir(root, i)) != NULL) {
        fsnode = fs_finddir(root, node->name);
        if (fsnode != NULL) {
            print_string(fsnode->name);
            print_string("\n");
        }

        i++;
    }
}

int list_directory(char* dir_name, FsNode_t* buf, int max, int ds) {
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
            if (buf != NULL) {
                buf++;
                seg_copy(fsnode, buf, sizeof(FsNode_t), DATA_SEGMENT, ds);
                seg_copy(fsnode->name, buf->name, strlen(fsnode->name),
                         DATA_SEGMENT, ds);
            }
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

int seg_copy(char* src, char* dst, int len, int src_seg, int dst_seg);
extern void make_break_interrupt();

int i21_handler(SyscallArgs_t* args) {
    const int argv_item_size = 64;  // 64 chars per arg

    switch (args->num) {
        case 1: {
            char name[128];

            char** argv_in;
            char** argv = malloc(args->b * sizeof(char));

            seg_copy(args->c, argv_in, args->b * sizeof(char*), args->ds,
                     DATA_SEGMENT);

            for (int i = 0; i < args->b; i++) {
                argv[i] = malloc(argv_item_size * sizeof(char));
                seg_copy(argv_in[i], argv[i], argv_item_size, args->ds,
                         DATA_SEGMENT);
            }

            seg_copy(args->a, name, sizeof(name), args->ds, DATA_SEGMENT);
            int ret = exec(name, args->b, argv, args->d, args->e, args->f, TRUE);

            for (int i = 0; i < args->b; i++) {
                free(argv[i]);
            }
            free(argv);
        } break;

        case 2: {
            char name[128];
            seg_copy(args->a, name, sizeof(name), args->ds, DATA_SEGMENT);

            return list_directory(name, args->b, args->c, args->ds);
        }

        case 3: {
            int read_len = args->c;
            char buf[SYSCALL_BUF_SIZ];
            char* dst = args->b;

            int ret = 0;
            int len;

            while (read_len >= SYSCALL_BUF_SIZ) {
                read_len -= SYSCALL_BUF_SIZ;

                len = read(args->a, buf, SYSCALL_BUF_SIZ);
                ret += len;

                seg_copy(buf, dst, len, DATA_SEGMENT, args->ds);

                dst += ret;
            }

            if (read_len) {
                len = read(args->a, buf, read_len);
                ret += len;
                seg_copy(buf, dst, read_len, DATA_SEGMENT, args->ds);
            }

            return ret;
        }

        case 4: {
            char text[SYSCALL_BUF_SIZ];
            seg_copy(args->b, text, sizeof(text), args->ds, DATA_SEGMENT);
            return write(args->a, text, args->c);
        }

        case 5: {
            char name[SYSCALL_BUF_SIZ];
            seg_copy(args->a, name, sizeof(name), args->ds, DATA_SEGMENT);
            return open(name);
        }

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

#define MUST_COMPLETE(method, success, error, ...) \
    if (method(__VA_ARGS__)) {                     \
        print_string(error);                       \
        return 1;                                  \
    } else                                         \
        print_string(success)

void ctrl_break() {
    print_string("Break");
}

void task() {}

int a20_init() {
    if (a20_available()) {
        int enable_fail;
        print_string("A20 line is available\n");

        MUST_COMPLETE(a20_enable, "A20 line successfully enabled\n",
                      "A20 line failed to enable\n");
    } else {
        print_string("A20 line is unavaiable\n");
    }

    return 0;
}

int init(struct SystemInfo* info) {
    cls();

    _lowmem = info->lowmem;
    _highmem = info->highmem;

    a20_init();

    MUST_COMPLETE(memmgr_init, "Memory manager enabled\n",
                  "Memory manager failed to initialise\n");

    make_break_interrupt();

    makeInterrupt21();
    print_string("Int 21h enabled\n");

    MUST_COMPLETE(rtc_init, "RTC enabled\n", "Failed to initialise rtc\n");

    MUST_COMPLETE(serial_init, "Enabled /dev/com1\n",
                  "Failed to initialise /dev/com1\n", COM1, BAUD_9600,
                  PARITY_NONE, STOPBITS_ONE, DATABITS_8);

    MUST_COMPLETE(serial_init, "Enabled /dev/com2\n",
                  "Failed to initialise /dev/com2\n", COM2, BAUD_9600,
                  PARITY_NONE, STOPBITS_ONE, DATABITS_8);

    fs_root = fat_init(info->rootfs_start);
    FsNode_t* fs_dev = devfs_init();
    fs_mount("dev", fs_root, fs_dev);
    print_string("Root filesystem mounted\n");

    stdin = open("/dev/stdin");
    stdout = open("/dev/stdout");
    stderr = open("/dev/stderr");

    char* shell_argv[] = {"shell", "login.bat"};
    exec("dir", 0, NULL, stdin, stdout, stderr, FALSE);
    exec("shell", 0, NULL, stdin, stdout, stderr, FALSE);

    close(stdin);
    close(stdout);
    close(stderr);

    return 0;
}
