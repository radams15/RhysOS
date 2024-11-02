#ifndef SERIAL_H
#define SERIAL_H

typedef enum Port { COM1 = 0, COM2 = 1, COM3 = 2 } Port_t;

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
    PARITY_NONE_2 = 2,
    PARITY_EVEN = 3
} Parity_t;

typedef enum StopBits { STOPBITS_ONE = 0, STOPBITS_TWO = 1 } StopBits_t;

typedef enum DataBits { DATABITS_7 = 2, DATABITS_8 = 3 } DataBits_t;


typedef void(*serial_init_t)(void);
typedef void(*serial_putc_t)(void);
typedef void(*serial_getc_t)(void);

typedef struct SerialDriver {
    serial_init_t init;
    serial_putc_t putc;
    serial_getc_t getc;
} SerialDriver_t;

int serial_init(int port,
                int baud,
                int parity,
                int stop_bits,
                int data_bits);

void serial_putc(int port, char c);
char serial_getc(int port);

#endif
