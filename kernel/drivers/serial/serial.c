#include "serial.h"
#include "util.h"

#ifndef SERIAL_ECHO
#define SERIAL_ECHO 1
#endif

#define UART_CLOCK 115200

typedef enum Baud {
    BAUD_110 = 0,
    BAUD_150 = 1,
    BAUD_300 = 2,
    BAUD_600 = 3,
    BAUD_1200 = 4,
    BAUD_2400 = 5,
    BAUD_4800 = 6,
    BAUD_9600 = 7
} Baud_t;

typedef enum Parity {
    PARITY_NONE = 0,
    PARITY_ODD = 1,
    PARITY_EVEN = 3,
} Parity_t;

int interrupt(int number, int AX, int BX, int CX, int DX);

static unsigned int port_map[] = {0x3f8, 0x2F8, 0x3E8};

static char calculate_stop_bit(int stop_bits) {
    switch(stop_bits) {
        case 0: return 0;
        case 1: return 1;
        default:
                printf("Invalid stop bit setting (%d), defaulting to 1\n", stop_bits);
                return 0;
    }
}
static char calculate_data_bit(int data_bits) {
    switch(data_bits) {
        case 0: return 0b00;
        case 1: return 0b01;
        case 2: return 0b10;
        case 3: return 0b11;
        default:
                printf("Invalid data bit setting (%d), defaulting to 8\n", data_bits);
                return 0b10;
    }
}

static char calculate_parity(int parity) {
    switch(parity) {
        case PARITY_NONE: return 0b000;
        case PARITY_ODD: return 0b001;
        case PARITY_EVEN: return 0b011;
        default:
                printf("Invalid parity setting (%d), defaulting to none\n", parity);
                return 0b000;
    }
}

static char calculate_baud(int baud) {
    static int lookup[] = {
        0x417, 0x300, 0x180, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x3
    };

    return lookup[baud];
}

int io_serial_init(int port,
                int baud,
                int parity,
                int stop_bits,
                int data_bits) {
    unsigned int portnum = port_map[port];

    unsigned int setting_byte = calculate_parity(parity) << 5
        | calculate_stop_bit(stop_bits) << 2
        | calculate_data_bit(data_bits);

    outb(portnum + 0, calculate_baud(baud));  // Set divisor to 3 (low byte) 38400 baud
    outb(portnum + 1, 0x00);  //                  (high byte)
    outb(portnum + 3, setting_byte);  // 8 bits, no parity, one stop bit
    outb(portnum + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
    outb(portnum + 4, 0x0B);  // IRQs enabled, RTS/DSR set
    outb(portnum + 4, 0x1E);  // Set in loopback mode, test the serial chip
    outb(portnum + 0, 0xAE);  // Test serial chip (send byte 0xAE and check if
                              // serial returns same byte)
    outb(portnum + 1, 0xFF);  // Enable all interrupts

    // Check if serial is faulty (i.e: not same byte as sent)
    // if (inb(portnum + 0) != 0xAE) {
    // return 1;
    // }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(portnum + 4, 0x0F);

    return 0;
}

void io_serial_putc(int port, char c) {
    if (c == '\n')
        outb(port_map[port], '\r');

    outb(port_map[port], c);
}

char io_serial_getc(int port) {
    int out;

    do {
        out = interrupt(0x14, (0x02 << 8), 0, 0, port);
    } while (out == 0);

#if SERIAL_ECHO
    serial_putc(port, out);
#endif

    return out;
}
