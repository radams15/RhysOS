#include "fs.h"

#include "util.h"
#include "proc.h"

extern int interrupt(int number, int AX, int BX, int CX, int DX);

void strcpy(char* to, char* from, int length) {
  int i;
  for(i = 0; i < length; i++) {
    *(to + i) = *(from + i);
  }
}


