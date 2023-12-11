#include <stdio.h>
#include <string.h>
#include <syscall.h>

#define MAX_PARAMS 16

static char* prompt = "A:/ ";

static int run_batch(char* path) {
    int fh;
    int totalread;
    int len;
    int c;
    char linebuf[1024];

    fh = open(path, NULL);

    if (fh == -1) {
        printf("Error reading file '%s'!\n", path);
        return 1;
    }

    totalread = 0;
    len = 0;

    while ((c = fgetch(fh)) != 0) {  // Each line
        linebuf[len] = c;
        len++;

        if (c == '\n') {
            linebuf[len - 1] = 0;
            if (linebuf[len - 2] == '\r')
                linebuf[len - 2] = 0;

            totalread += len;
            len = 0;

            run_line(linebuf, len);
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

    tok = strtok(rest, " ");

    while (tok != NULL) {
        *(tok - 1) = 0;  // null terminate section (replacing space)

        if (strcmp(tok, ">") == 0) {  // redirection?
            tok = strtok(NULL, ">");
            dest = tok;
            *(tok - 1) = 0;
            out_fh = open(dest, O_CREAT);

            if (out_fh == -1)
                out_fh = stdout;

            err_fh = out_fh;
            break;
        }

        if (strcmp(tok, "<") == 0) {  // redirection?
            tok = strtok(NULL, "<");
            dest = tok;
            *(tok - 1) = 0;
            in_fh = open(dest, NULL);

            if (in_fh == -1)
                in_fh = stdin;

            break;
        }

        argv[argc++] = tok;
        tok = strtok(NULL, " ");
    }

    if (endswith(exe, ".bat")) {
        return run_batch(exe);
    } else {
        return execa(exe, argc, argv, in_fh, out_fh, err_fh);
    }
}

int run_line(char* line, int length) {
    char* tok;
    char* exe;

    tok = strtok(line, " ");

    exe = tok;

    if (strcmp(exe, "tty") == 0) {
        int fh;

        fh = open(line + strlen(exe) + 1, NULL);

        if (fh == -1)
            printf("Error: File '%s' does not exist!\n",
                   line + strlen(exe) + 1);
        else
            stdout = stdin = stderr = fh;
    } else if (strcmp(exe, "exit") == 0) {
        return -1;
    } else {
        run_external(exe, line + strlen(exe) + 1);
    }
}

int loop() {
    char line[1024];

    memset(&line, 0, 1024 / 8);

    printf(prompt);
    int len = readline(line);

    printf("\n");

    if (len == 0)
        return 0;

    int ret = run_line(line, sizeof(line));

    if (ret == -1)
        return 1;

    printf("\n");

    return 0;
}

int main() {
    char name[32];
    int ret = 0;

    while (!ret)
        ret = loop();

    return 0;
}
