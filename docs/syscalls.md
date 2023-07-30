# Syscall Table:

## 1 - exec

`int exec(char* file, int argc, char** argv, int in, int out, int err);`

## 2 - list\_directory

`int list_directory(char* dir_name, struct FsNode* buf, int max);`

## 3 - read

`int read(int fh, unsigned char* buffer, unsigned int size);`

## 4 - write

`int write(int fh, unsigned char* buffer, unsigned int size);`

## 5 - open

`int open(char* name);`

## 6 - close

`void close(int fh);`

## 7 - seek

`void seek(int fh, unsigned int location);`
