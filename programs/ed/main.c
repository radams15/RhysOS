#include <stdio.h>

#include "buffer.h"

#define buffer make_pl
// const char buffer[] = ""; 

typedef enum Command {
    CMD_ESC,
    CMD_CHAR,
    CMD_INSERT,
    CMD_QUIT,
    CMD_PL,
    CMD_NL
} Command_t;

typedef enum Mode {
    MODE_CMD,
    MODE_EDIT
} Mode_t;

typedef struct Ctx {
    Mode_t mode;
    char buf[8];
    char bufptr;
} Ctx_t;

Command_t parse(Ctx_t* ctx, char c) {
    if(ctx->mode == MODE_EDIT) {
        switch(c) {
            case 0x1B:
                return CMD_ESC;
            default:
                return CMD_CHAR;
        }
    }

    // In CMD mode

    switch(c) {
        case 'n': return CMD_NL;
        case 'p': return CMD_PL;

        case 'i':
            return CMD_INSERT;

        case 'q':
            return CMD_QUIT;

        default:
            return CMD_ESC;
    }
};

int main() {
    Ctx_t ctx;
    ctx.bufptr = &ctx.buf;
    ctx.mode = MODE_CMD;
    char c[2] = {0};

    while(TRUE) {
        c[0] = ngetch();
        switch(parse(&ctx, c[0])) {
            case CMD_CHAR:
                // Add char
                printf("%d", c[0]);
                break;
            case CMD_ESC:
                ctx.mode = MODE_CMD;
                break;
            case CMD_INSERT:
                ctx.mode = MODE_EDIT;
                break;
            case CMD_QUIT:
                goto quit;
            default:
                fprintf(stderr, "Input error, exiting...");
                goto error;
        }
    }

quit:
    return 0;

error:
    return 1;
}
