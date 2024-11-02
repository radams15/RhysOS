#ifndef SERIAL_SERIAL_H
#define SERIAL_SERIAL_H


int serial_init(int port,
                int baud,
                int parity,
                int stop_bits,
                int data_bits);

void serial_putc(int port, char c);
char serial_getc(int port);

#endif
