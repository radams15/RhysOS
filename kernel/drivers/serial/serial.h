#ifndef SERIAL_SERIAL_H
#define SERIAL_SERIAL_H


int io_serial_init(int port,
                int baud,
                int parity,
                int stop_bits,
                int data_bits);

void io_serial_putc(int port, char c);
char io_serial_getc(int port);

#endif
