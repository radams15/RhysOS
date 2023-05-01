#include "util.h"
#include "type.h"

void memset(int *dest, int val, int len) {
    int *temp = (int *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

void strcpy(char* a, char* b) {
	memcpy(a, b, strlen(b));
}

int div (int a, int b) {
  int res = 0;
  while (a>=b) {
    a -= b;
    res++;
  }
  return res;
}

int mod (int a, int n) {
	return a - div(a,n) * n;
}

void memcpy(char* dest, char* src, unsigned int n) {
	int i;
	for(i=0; i<n ; i++) {
		dest[i] = src[i];
	}
}

void clear(int *ptr, unsigned int n) {
	memset(ptr, 0, n);
}

int strlen(char* str) {
	int i = 0;
	char* c;
	
	for(c=str ; *c != 0 ; c++) {
		i++;
	}
	
	return i;
}


int strncmp(char* a, char*b, int length) {
	int i;
	
	for(i=0 ; i<length ; i++) {
		if(a[i] != b[i])
			return 1;
	}
	
	return 0;
}

int strcmp(char* a, char* b) {
	return strncmp(a, b, strlen(a));
}

int oct2bin(unsigned char* str, int size) {
    int n = 0;
    unsigned char *c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

char* strtok(register char* s, register char* delim) {
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;


	if (s == NULL && (s = last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
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
		spanp = (char *)delim;
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

