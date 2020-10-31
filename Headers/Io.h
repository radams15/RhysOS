//
// Created by rhys on 30/10/2020.
//

#ifndef RHYSOS_IO_H
#define RHYSOS_IO_H

#include "Types.h"

namespace Io{
    void outb(uint16 port, int8 val);
    uint8 inb(uint16 port);

    void halt();
}

#endif //RHYSOS_IO_H
