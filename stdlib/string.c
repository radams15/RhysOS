#include "string.h"
#include "stddef.h"

int strlen(char* str) {
    int i = 0;
    char* c;

    for (c = str; *c != 0; c++) {
        i++;
    }

    return i;
}

int strncmp(char* a, char* b, int length) {
    int i;

    for (i = 0; i < length; i++) {
        if (a[i] != b[i])
            return 1;
    }

    return 0;
}

int strcmp(char* a, char* b) {
    return strncmp(a, b, strlen(a));
}

int endswith(char* a, char* suffix) {
    char* a_end;
    char* suff_end;

    a_end = a + (strlen(a) - 1);
    suff_end = suffix + (strlen(suffix) - 1);

    for (; suff_end >= suffix;) {
        if (*a_end != *suff_end)
            return 0;

        suff_end--;
        a_end--;
    }

    return 1;
}

int strcpy(char* dst, char* src) {
    return strncpy(dst, src, strlen(src));
}

int strncpy(char* dst, char* src, int length) {
    for (int i = 0; i < length; i++) {
        dst[i] = src[i];
    }

    return length;
}

char* strtok(register char* s, register char* delim) {
    register char* spanp;
    register int c, sc;
    char* tok;
    static char* last;

    if (s == NULL && (s = last) == NULL)
        return (NULL);

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
cont:
    c = *s++;
    for (spanp = (char*)delim; (sc = *spanp++) != 0;) {
        if (c == sc)
            goto cont;
    }

    if (c == 0) { /* no non-delimiter characters */
        last = NULL;
        return (NULL);
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;) {
        c = *s++;
        spanp = (char*)delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                last = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}

int atoi(char* str) {
    // Initialize result
    int res = 0;

    // Initialize sign as positive
    int sign = 1;

    // Initialize index of first digit
    int i = 0;

    // If number is negative,
    // then update sign
    if (str[0] == '-') {
        sign = -1;

        // Also update index of first digit
        i++;
    }

    // Iterate through all digits
    // and update the result
    for (; str[i] != ' '; ++i)
        res = res * 10 + str[i] - '0';

    // Return result with sign
    return sign * res;
}

#define INT_MAX 32767
#define INT_MIN -32767

int strtoi(const char* nptr, char** endptr, int base) {
    const char *p = nptr, *endp;
    _Bool is_neg = 0, overflow = 0;
    /* Need unsigned so (-INT_MIN) can fit in these: */
    unsigned int n = 0, cutoff;
    int cutlim;
    if (base < 0 || base == 1 || base > 36) {
        return 0;
    }
    endp = nptr;
    while (*p == ' ')
        p++;
    if (*p == '+') {
        p++;
    } else if (*p == '-') {
        is_neg = 1, p++;
    }
    if (*p == '0') {
        p++;
        /* For strtol(" 0xZ", &endptr, 16), endptr should point to 'x';
         * pointing to ' ' or '0' is non-compliant.
         * (Many implementations do this wrong.) */
        endp = p;
        if (base == 16 && (*p == 'X' || *p == 'x')) {
            p++;
        } else if (base == 2 && (*p == 'B' || *p == 'b')) {
            /* C23 standard supports "0B" and "0b" prefixes. */
            p++;
        } else if (base == 0) {
            if (*p == 'X' || *p == 'x') {
                base = 16, p++;
            } else if (*p == 'B' || *p == 'b') {
                base = 2, p++;
            } else {
                base = 8;
            }
        }
    } else if (base == 0) {
        base = 10;
    }
    cutoff = (is_neg) ? -(INT_MIN / base) : INT_MAX / base;
    cutlim = (is_neg) ? -(INT_MIN % base) : INT_MAX % base;
    while (1) {
        int c;
        if (*p >= 'A')
            c = ((*p - 'A') & (~('a' ^ 'A'))) + 10;
        else if (*p <= '9')
            c = *p - '0';
        else
            break;
        if (c < 0 || c >= base)
            break;
        endp = ++p;
        if (overflow) {
            /* endptr should go forward and point to the non-digit character
             * (of the given base); required by ANSI standard. */
            if (endptr)
                continue;
            break;
        }
        if (n > cutoff || (n == cutoff && c > cutlim)) {
            overflow = 1;
            continue;
        }
        n = n * base + c;
    }
    if (endptr)
        *endptr = (char*)endp;
    if (overflow) {
        return ((is_neg) ? INT_MIN : INT_MAX);
    }
    return (int)((is_neg) ? -n : n);
}
