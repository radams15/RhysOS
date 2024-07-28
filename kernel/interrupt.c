#include "interrupt.h"
#include "clock.h"
#include "fs/fs.h"
#include "keyboard.h"
#include "malloc.h"
#include "proc.h"
#include "tty.h"
#include "type.h"
#include "util.h"

#define SYSCALL_BUF_SIZ 1024

typedef struct SyscallArgs {
    int num;  // Syscall number
    int a, b, c, d, e, f;
    int cs, ds;  // Data segment
} SyscallArgs_t;

int i21_handler(SyscallArgs_t* args) {
    const int argv_item_size = 64;  // 64 chars per arg

    switch (args->num) {
        case 1: {
            char name[128];

            // argc = args->b, argv = args->c

            char** argv =
                malloc(args->b * sizeof(char));  // array of args to populate

            if(argv == NULL) {
                print_string("Unable to malloc argv[i]\n");
                return 2;
            }

            seg_copy((char*)args->c, (char*)argv, args->b * sizeof(char*),
                     args->ds,
                     KERNEL_SEGMENT);  // copy argv pointers themselves, i.e.
                                     // addresses of the argv elements

            for (int i = 0; i < args->b; i++) {
                char* addr = argv[i];  // address in calling segment
                argv[i] = malloc(argv_item_size *
                                 sizeof(char));  // allocate space for the
                                                 // argument in kernel segment
                if(argv[i] == NULL) {
                    print_string("Unable to malloc argv[i]\n");
                    return 2;
                }

                seg_copy(addr, argv[i], argv_item_size, args->ds, KERNEL_SEGMENT);
            }

            seg_copy((char*)args->a, (char*)name, sizeof(name), args->ds,
                     KERNEL_SEGMENT);
            int ret =
                exec(name, args->b, argv, args->d, args->e, args->f, FALSE);

            for (int i = 0; i < args->b; i++) {
                free(argv[i]);
            }
            free(argv);

            return ret;
        } break;

        case 2: {
            char name[128];
            seg_copy((char*)args->a, (char*)name, sizeof(name), args->ds,
                     KERNEL_SEGMENT);

            return list_directory(name, (FsNode_t*)args->b, args->c, args->ds);
        }

        case 3: {
            int read_len = args->c;
            char buf[SYSCALL_BUF_SIZ];
            char* dst = (char*)args->b;

            int ret = 0;
            int len;

            while (read_len >= SYSCALL_BUF_SIZ) {
                read_len -= SYSCALL_BUF_SIZ;

                len = read(args->a, (unsigned char*)buf, SYSCALL_BUF_SIZ);
                ret += len;

                seg_copy((char*)buf, dst, len, KERNEL_SEGMENT, args->ds);

                dst += ret;
            }

            if (read_len) {
                len = read(args->a, (unsigned char*)buf, read_len);
                ret += len;
                seg_copy((char*)buf, dst, read_len, KERNEL_SEGMENT, args->ds);
            }

            return ret;
        }

        case 4: {
            char text[SYSCALL_BUF_SIZ];
            seg_copy((char*)args->b, (char*)text, sizeof(text), args->ds,
                     KERNEL_SEGMENT);
            return write(args->a, (unsigned char*)text, args->c);
        }

        case 5: {
            char name[SYSCALL_BUF_SIZ];
            seg_copy((char*)args->a, (char*)name, sizeof(name), args->ds,
                     KERNEL_SEGMENT);
            return open(name, args->b);
        }

        case 6:
            close(args->a);
            break;

        case 7:
            seek(args->a, args->b);
            break;

        case 8:
            print_string("Free: ");printi(args->a, 16);print_char('\n');
            free((void*)args->a);
            break;

        case 9: {
            char name[SYSCALL_BUF_SIZ];
            Stat_t dst;

            seg_copy((char*)args->a, (char*)name, sizeof(name), args->ds,
                     KERNEL_SEGMENT);

            int ret = stat(name, &dst);

            if (ret == 0) {
                seg_copy((char*)&dst, (char*)args->b, sizeof(Stat_t),
                         KERNEL_SEGMENT, args->ds);
            }

            return ret;
        }

        default:
            print_string("Unknown interrupt: ");
            printi(args->num, 16);
            print_string("!\r\n");
            return -1;
            break;
    }

    return 0;
}

int handle_interrupt_21(int* ax, int ss, int cx, int dx) {
    SyscallArgs_t arg_data;
    seg_copy((char*)ax, (char*)&arg_data, sizeof(SyscallArgs_t), ss,
             KERNEL_SEGMENT);

    arg_data.num = i21_handler(&arg_data);

    seg_copy((char*)&arg_data, (char*)ax, sizeof(SyscallArgs_t), KERNEL_SEGMENT,
             ss);

    return 0;
}

void ctrl_break() {
    print_string("Break");
}

// https://wiki.osdev.org/Interrupt_Vector_Table#CPU_Interrupt_Layout
enum Interrupt {
    INTR_DIV0 = 0x00,
    INV_OPCODE = 0x06,
    KBD = 0x24,
    INTR_BREAK = 0x6c,
    INTR_TICK = 0x70
};

void handle_interrupt(enum Interrupt code) {
    switch (code) {
        case INTR_DIV0:
            print_string("Division by zero!");
            break;
        case INV_OPCODE:
            print_string("Invalid Opcode!");
            break;
        case INTR_BREAK:
            ctrl_break();
            break;

        case KBD: {
            char c = inb(0x60);

            kbd_key_press(c);

            outb(0x20, 0x61);
        } break;
        case INTR_TICK:
            clock_tick();
            break;
        default:
            print_string("Unknown interrupt: ");
            printi(code, 16);
            print_string("\n");
            break;
    }
}
