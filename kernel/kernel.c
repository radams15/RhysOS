void main(int src_ds, void* boot_ptr);
void entry(int src_ds, void* boot_ptr) {
    main(src_ds, boot_ptr);
}

#include "fs/fs.h"
#include "interrupt.h"
#include "malloc.h"
#include "proc.h"
#include "sysinfo.h"
#include "tty.h"
#include "util.h"

#include "fs/devfs.h"
#include "fs/fat.h"

#include "clock.h"
#include "serial.h"

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

void task() {}

#define MUST_COMPLETE(method, success, error, ...) \
    if (method(__VA_ARGS__)) {                     \
        print_string(error);                       \
        return 1;                                  \
    } else                                         \
        print_string(success)

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

void mouse_tick() {
    // print_string("t");
    // char mouse_bytes[8];
    //
    // for(int i=0 ; i<4 ; i++) {
    // mouse_bytes[i] = serial_getc(COM1);
    // printi(mouse_bytes[i], 16);
    // }
}

int init(struct SystemInfo* info) {
    _lowmem = info->lowmem;
    _highmem = info->highmem;

    graphics_init();

    cls();
    
    a20_init();


    MUST_COMPLETE(memmgr_init, "Memory manager enabled\n",
                  "Memory manager failed to initialise\n");

    makeInterrupt21();
    print_string("Int 21h enabled\n");

    MUST_COMPLETE(rtc_init, "RTC enabled\n", "Failed to initialise rtc\n");

    MUST_COMPLETE(serial_init, "Enabled /dev/com1\n",
                  "Failed to initialise /dev/com1\n", COM1, BAUD_9600,
                  PARITY_NONE, STOPBITS_ONE, DATABITS_8);

    // MUST_COMPLETE(serial_init, "Enabled /dev/com2\n",
    //               "Failed to initialise /dev/com2\n", COM2, BAUD_9600,
    //               PARITY_NONE, STOPBITS_ONE, DATABITS_8);

    MUST_COMPLETE(init_interrupts, "Interrupts enabled\n",
                  "Interrupts failed to initialise\n");

    struct TimeDelta time_val;
    time(&time_val);
    srand(time_val.tick + time_val.sec + time_val.min + time_val.hr);

    fs_root = fat_init(info->rootfs_start);
    FsNode_t* fs_dev = devfs_init();
    FsNode_t* fs_tmp = tmpfs_init();
    fs_mount("dev", fs_root, fs_dev);
    fs_mount("tmp", fs_root, fs_tmp);
    print_string("Root filesystem mounted\n");

    stdin = open("/dev/stdin", O_RDONLY);
    stdout = open("/dev/stdout", O_WRONLY);
    stderr = open("/dev/stderr", O_WRONLY);

    // add_tick_callback(mouse_tick);

    char* shell_argv[] = {"shell", "login.bat"};
    exec("shell", 2, shell_argv, stdin, stdout, stderr, FALSE);

    close(stdin);
    close(stdout);
    close(stderr);

    return 0;
}
