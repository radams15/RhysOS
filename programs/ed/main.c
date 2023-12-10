#include <stdio.h>
#include <syscall.h>

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
    char line[8];
    char* lptr;
    int llength;
    char* bufptr; 
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
    ctx.bufptr = &buffer;
    ctx.lptr = &ctx.line;
    ctx.llength = 0;
    ctx.mode = MODE_CMD;
    char c = 0;

    while(TRUE) {
        putc('\r');
        for(int i=0 ; i<ctx.llength ; i++)
            putc(' ');
        putc('\r');

        ctx.llength = 0;
        for(char* c=ctx.bufptr ; *c != '\n' && *c != 0 ; c++) {
            putc(*c);
            ctx.llength++;
        }

        c = ngetch() & 0xFF;
        switch(parse(&ctx, c)) {
            case CMD_CHAR:
                // Add char
                break;
            case CMD_NL: {
                while(*ctx.bufptr != '\n' && *ctx.bufptr != 0) {
                    ctx.bufptr++;
                }
                ctx.bufptr++;
            }; break;
            case CMD_PL: {
                ctx.bufptr--;
                ctx.bufptr--;
                while(*ctx.bufptr != '\n' && ctx.bufptr != buffer) {
                    ctx.bufptr--;
                }
                ctx.bufptr++;
            }; break;
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
