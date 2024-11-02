#include "fs/fs.h"
#include "interrupt.h"
#include "malloc.h"
#include "pmode.h"
#include "proc.h"
#include "rand.h"
#include "sysinfo.h"
#include "tty.h"
#include "util.h"

#include "fs/devfs.h"
#include "fs/fat.h"
#include "fs/tmpfs.h"

#include "clock.h"
#include "drivers/serial/serial.h"

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

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

    seg_copy((char*)boot_ptr, (char*)&info, sizeof(struct SystemInfo), src_ds,
             KERNEL_SEGMENT);

    err = init(&info);

    if (err) {
        print_string("\r\nError in kernel, halting!\r\n");
    }

    for (;;) {
    }
}

void debug(const char* label, int data) {
    print_string((char*)label);
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
        return;
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

#define MUST_COMPLETE(method, success, error, ...) \
    if (method(__VA_ARGS__)) {                     \
        print_string(error);                       \
        return 1;                                  \
    } else                                         \
        print_string(success)

int a20_init() {
    if (a20_available()) {
        print_string("A20 line is available\n");

        MUST_COMPLETE(a20_enable, "A20 line successfully enabled\n",
                      "A20 line failed to enable\n");
    } else {
        print_string("A20 line is unavaiable\n");
    }

    return 0;
}

void mouse_tick() {
    print_string("t");
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

    print_string("Booting RhysOS, version ");
    print_string(STRINGIZE(RELEASE_VERSION));
    print_string("\n");

    a20_init();

    MUST_COMPLETE(memmgr_init, "Memory manager enabled\n",
                  "Memory manager failed to initialise\n");

    make_interrupt_21();
    print_string("Int 21h enabled\n");

    MUST_COMPLETE(rtc_init, "RTC enabled\n", "Failed to initialise rtc\n");

    MUST_COMPLETE(serial_init, "Enabled /dev/com1\n",
                  "Failed to initialise /dev/com1\n", COM1, BAUD_9600,
                  PARITY_NONE, STOPBITS_ONE, DATABITS_8);

    MUST_COMPLETE(serial_init, "Enabled /dev/com2\n",
                  "Failed to initialise /dev/com2\n", COM2, BAUD_9600,
                  PARITY_NONE, STOPBITS_ONE, DATABITS_8);

    MUST_COMPLETE(init_interrupts, "Interrupts enabled\n",
                  "Interrupts failed to initialise\n");

    struct TimeDelta time_val;
    time(&time_val);
    srand(time_val.tick + time_val.sec + time_val.min + time_val.hr);

    fs_root = fat_init(info->rootfs_start);
    FsNode_t* fs_dev = devfs_init();
    // FsNode_t* fs_tmp = tmpfs_init();
    fs_mount("dev", fs_root, fs_dev);
    // fs_mount("tmp", fs_root, fs_tmp);
    print_string("Root filesystem mounted\n");

    stdin = open("/dev/stdin", O_RDONLY);
    stdout = open("/dev/stdout", O_WRONLY);
    stderr = open("/dev/stderr", O_WRONLY);

    char* shell_argv[] = {"shell", "login.bat"};
    // exec("shell", 2, shell_argv, stdin, stdout, stderr, FALSE);
    exec("shell", 0, NULL, stdin, stdout, stderr, FALSE);

    close(stdin);
    close(stdout);
    close(stderr);

    return 0;
}
