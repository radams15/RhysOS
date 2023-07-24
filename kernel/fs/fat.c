#include "fat.h"

#include "stdio.h"

short fat_sector[512];
 

void fat_init(int sector_start) {
  read_sector(&fat_sector, sector_start);
  
  
}
