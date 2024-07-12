#include <stdio.h>

#define CHUNK_SIZE \
    15  // Read 15 chars at a time (one line of hex + the actual text)

int dump(char* file) {
    int n;
    char buffer[CHUNK_SIZE + 1];
    int fh;
    int bytes_read = 1;
    int total_read = 0;
    int i;

    fh = open(file, NULL);

    if (fh == -1) {
        printf("Error reading file '%s'!\n", file);
        return 1;
    }

    do {
        seek(fh, total_read, SEEK_SET);

        bytes_read = read(fh, &buffer, CHUNK_SIZE);

        if (bytes_read == 0)
            break;

        for (i = 0; i < bytes_read; i++) {
            printf("%x ", buffer[i]);
        }
        printf(" =>\t");
        for (i = 0; i < bytes_read; i++) {
            if (buffer[i] > 33)  // Printable characters printed
                putc(buffer[i]);
            else  // Non-printable characters print '.'
                putc('.');
        }
        printf("\n");
        total_read += bytes_read;
    } while (bytes_read > 0);

    close(fh);

    return 0;
}

int main(int argc, char** argv) {
    int i;

    for (i = 1; i < argc; i++) {
        dump(argv[i]);
    }

    return 0;
}
