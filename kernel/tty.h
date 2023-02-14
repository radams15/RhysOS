#ifndef RHYSOS_TTY_H
#define RHYSOS_TTY_H


void clear_screen();

void set_resolution(int mode);

void print_char(c);

void print_string(int* str);
void print_hex(int i);
void set_cursor(int col, int row);

int getch();

#endif
