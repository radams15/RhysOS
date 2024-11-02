#include "serial.h"

#include "drivers/serial/serial.h"

struct SerialDriver serial_driver;

int serial_load() {
    serial_driver.init = io_serial_init;
    serial_driver.putc = io_serial_putc;
    serial_driver.getc = io_serial_getc;

    return 0;
}

int serial_init_port(Port_t port,
                Baud_t baud,
                Parity_t parity,
                StopBits_t stop_bits,
                DataBits_t data_bits) {
    return serial_driver.init(port, baud, parity, stop_bits, data_bits);
}

int serial_putc(Port_t port, char c) {
    return serial_driver.putc(port, c);
}

char serial_getc(Port_t port) {
    return serial_driver.getc(port);
}
