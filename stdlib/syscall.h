#ifndef SYSCALL_H
#define SYSCALL_H

#define SYSCALL(ax, bx, cx, dx) interrupt(0x21, ax, bx, cx, dx)

extern int interrupt (int number, int AX, int BX, int CX, int DX);

int print(char* str);
int readline(char* buffer);
int exec(char* file);
int sys_set_graphics_mode(int mode);

#endif
