#include "serial.h"

int interrupt (int number, int AX, int BX, int CX, int DX);

void serial_init(Port_t port, Baud_t baud, Parity_t parity, StopBits_t stop_bits, DataBits_t data_bits) {
	int code;
	code = (baud<<4) + (parity << 2) + (stop_bits << 1) + (data_bits);
	
	interrupt(0x14, (0x00<<8) + code, 0, 0, port);
}

void serial_putc(Port_t port, char c) {
	interrupt(0x14, (0x01<<8) + c, 0, 0, port);
}

char serial_getc(Port_t port) {
	int out = interrupt(0x14, (0x02<<8), 0, 0, port);

#if SERIAL_ECHO
	serial_putc(port, out);
#endif
	
	return out;
}
