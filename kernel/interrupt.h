#ifndef INTERRUPT_H
#define INTERRUPT_H

int init_interrupts();
int make_interrupt_21();
int interrupt(int number, int AX, int BX, int CX, int DX);

#endif
