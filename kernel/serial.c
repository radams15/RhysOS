#include "serial.h"
#include "util.h"

#ifndef SERIAL_ECHO
#define SERIAL_ECHO 1
#endif

#ifndef BIOS_SERIAL
#define BIOS_SERIAL 0
#endif

int interrupt(int number, int AX, int BX, int CX, int DX);

#if BIOS_SERIAL

int serial_init(Port_t port,
                Baud_t baud,
                Parity_t parity,
                StopBits_t stop_bits,
                DataBits_t data_bits) {
    int code;
    code = (baud << 4) + (parity << 2) + (stop_bits << 1) + (data_bits);

    interrupt(0x14, (0x00 << 8) + code, 0, 0, port);

    return 0;
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





unsigned int port_map[] = {0x3f8, 0x2F8};

int serial_init(Port_t port,
                Baud_t baud,
                Parity_t parity,
                StopBits_t stop_bits,
                DataBits_t data_bits) {
                
    unsigned int portnum = port_map[port];
                
    outb(portnum + 0, 0x03);    // Set divisor to 3 (low byte) 38400 baud
    outb(portnum + 1, 0x00);    //                  (high byte)
    outb(portnum + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(portnum + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(portnum + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(portnum + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(portnum + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
    outb(portnum + 1, 0xFF);    // Enable all interrupts
    
    // Check if serial is faulty (i.e: not same byte as sent)
    if(inb(portnum + 0) != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(portnum + 4, 0x0F);
    
    return 0;
}

void serial_putc(Port_t port, char c) {
    if (c == '\n')
        outb(port_map[port], '\r');

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
