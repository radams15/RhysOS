#include "serial.h"

#ifndef SERIAL_ECHO
#define SERIAL_ECHO 1
#endif

#ifndef BIOS_SERIAL
#define BIOS_SERIAL 1
#endif

int interrupt(int number, int AX, int BX, int CX, int DX);

#if BIOS_SERIAL

void serial_init(Port_t port,
                 Baud_t baud,
                 Parity_t parity,
                 StopBits_t stop_bits,
                 DataBits_t data_bits) {
    int code;
    code = (baud << 4) + (parity << 2) + (stop_bits << 1) + (data_bits);

    interrupt(0x14, (0x00 << 8) + code, 0, 0, port);
}

void serial_putc(Port_t port, char c) {
    if (c == '\n')
        serial_putc(port, '\r');

    interrupt(0x14, (0x01 << 8) + c, 0, 0, port);
}

char serial_getc(Port_t port) {
    int out;

    do {
        out = interrupt(0x14, (0x02 << 8), 0, 0, port);
    } while (out == 0);

#if SERIAL_ECHO
    serial_putc(port, out);
#endif

    return out;
}

#else

char port_map[] = {0x3F8, 0x2F8};

void serial_init(Port_t port,
                 Baud_t baud,
                 Parity_t parity,
                 StopBits_t stop_bits,
                 DataBits_t data_bits) {
    outb(port_map[port] + 1, 0x00);
    outb(port_map[port] + 3, 0x80);
    outb(port_map[port] + 0, 0x03);
    outb(port_map[port] + 1, 0x00);
    outb(port_map[port] + 3, 0x03);
    outb(port_map[port] + 2, 0xC7);
    outb(port_map[port] + 4, 0x0B);
    outb(port_map[port] + 4, 0x1E);

    outb(port_map[port] + 4, 0x0F);
}

void serial_putc(Port_t port, char c) {
    outb(port_map[port], c);
}

char serial_getc(Port_t port) {
    int out;

    do {
        out = interrupt(0x14, (0x02 << 8), 0, 0, port);
    } while (out == 0);

#if SERIAL_ECHO
    serial_putc(port, out);
#endif

    return out;
}

#endif