#include "string.hpp"

extern "C" {
#include <stdio.h>
#include <string.h>
#include <syscall.h>
}

#define MAX_PARAMS 16

static char* prompt = "A:/ ";

int run_line(string& line);

static int run_batch(char* path) {
    int fh;
    int totalread;
    int len;
    int c;
    string line(1024);

    fh = open(path);

    if (fh == -1) {
        printf("Error reading file '%s'!\n", path);
        return 1;
    }

    totalread = 0;
    len = 0;

    while ((c = fgetch(fh)) != 0) {  // Each line
        line.setc(len, c);
        len++;

        if (c == '\n') {
            line.setc(len - 1, 0);
            if (line.c_str()[len - 2] == '\r')
                line.setc(len - 2, 0);

            totalread += len;
            len = 0;

            run_line(line);
        }

        seek(fh, totalread + len);
    }

    close(fh);
}

static int run_external(char* exe, char* rest) {
    int argc;
    char* argv[MAX_PARAMS];
    char* tok;
    char* dest = NULL;

    int err_fh = stderr;
    int out_fh = stdout;
    int in_fh = stdin;

    argc = 1;
    argv[0] = exe;

    if (rest != NULL) {
        tok = (char*)strtok(rest, " ");

        while (tok != NULL) {
            *(tok - 1) = 0;  // null terminate section (replacing space)

            if (strcmp(tok, ">") == 0) {  // redirection?
                tok = (char*)strtok(NULL, ">");
                dest = tok;
                *(tok - 1) = 0;
                out_fh = open(dest);

                if (out_fh == -1)
                    out_fh = stdout;

                err_fh = out_fh;
                break;
            }

            if (strcmp(tok, "<") == 0) {  // redirection?
                tok = (char*)strtok(NULL, "<");
                dest = tok;
                *(tok - 1) = 0;
                in_fh = open(dest);

                if (in_fh == -1)
                    in_fh = stdin;

                break;
            }

            argv[argc++] = tok;
            tok = strtok(NULL, " ");
        }
    }

    if (endswith(exe, ".bat")) {
        return run_batch(exe);
    } else {
        return execa(exe, argc, argv, in_fh, out_fh, err_fh);
    }
}

int run_line(string& line) {
    char* tok;
    char* exe;
    
    char* line_copy = (char*) line.c_copy();

    tok = strtok(line_copy, " ");

    exe = tok;

    if (strcmp(exe, "tty") == 0) {
        int fh;

        fh = open(line_copy + strlen(exe) + 1);

        if (fh == -1)
            printf("Error: File '%s' does not exist!\n",
                   line_copy + strlen(exe) + 1);
        else
            stdout = stdin = stderr = fh;
    } else if (strcmp(exe, "exit") == 0) {
        return -1;
    } else {
        run_external(exe, line_copy + strlen(exe) + 1);
    }
}

int loop() {
    string line(1024);

    printf("%s", prompt);
    int len = readline(line.rbuf);

    printf("\n");

    if (len == 0)
        return 0;

    int ret = run_line(line);

    if (ret == -1)
        return 1;

    printf("\n");

    return 0;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            run_external(argv[i], NULL);
        }
    } else {
        int ret = 0;
        while (!ret)
            ret = loop();
    }

    return 0;
}
