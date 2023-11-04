#include "interrupt.h"
#include "type.h"

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
            
        case 8:
            free(args->a);
            break;

        default:
            print_string("Unknown interrupt: ");
            printi(args->num, 16);
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


void ctrl_break() {
    print_string("Break");
}

// https://wiki.osdev.org/Interrupt_Vector_Table#CPU_Interrupt_Layout
enum Interrupt {
    INTR_DIV0 = 0x00,
    INV_OPCODE = 0x06,
    INTR_BREAK = 0x6c,
    INTR_TICK = 0x70
};

void handle_interrupt(enum Interrupt code) {
    switch(code) {
        case INTR_BREAK:
            ctrl_break();
            break;
        case INTR_TICK:
            tick();
            break;
        default:
            print_string("Unknown interrupt: ");
            printi(code, 16);
            print_string("\n");
            break;
    }
}
