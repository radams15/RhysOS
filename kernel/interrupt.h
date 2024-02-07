#ifndef INTERRUPT_H
#define INTERRUPT_H

extern int init_interrupts();

int interrupt(int num, int AX, int BX, int CX, int DX);

int interrupt_10(int AX, int BX, int CX, int DX);
int interrupt_13(int AX, int BX, int CX, int DX);
int interrupt_14(int AX, int BX, int CX, int DX);
int interrupt_15(int AX, int BX, int CX, int DX);
int interrupt_16(int AX, int BX, int CX, int DX);

#endif
