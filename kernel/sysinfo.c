#include "sysinfo.h"

int _highmem = 0;
int _lowmem  = 0;

int highmem() {
    return _highmem;
}

int lowmem() {
    return _lowmem;
}
