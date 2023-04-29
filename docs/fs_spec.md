# FS Specification

## First sector (bytes 0-512):

Disk map for sectors.

Each byte refers to a sector of that index.

Either contains:

 - 0    - free
 - 0xFF - in use
 
```c
typedef char DiskMap[512];
```

# Next sector (bytes 512-1024):

Directory table.

Every 32 bytes can store 1 file name.

32 bytes contains:

 - File name padded by zeroes up to 8 bytes.
 - Sector number.
 
```c
struct DirEnt {
	char file_name[7];
	char ext[3];
	int sector;
};

typedef struct DirEnt DirectoryTable[16]; // 512/32 = 16
```

# Rest of sectors

Filled with file data.
