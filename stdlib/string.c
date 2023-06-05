#include "string.h"
#include "stddef.h"

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

int endswith(char* a, char* suffix) {
    char* a_end;
    char* suff_end;
    
    a_end = a+(strlen(a)-1);
    suff_end = suffix+(strlen(suffix)-1);
    
    for(;suff_end >= suffix ; ) {
        if(*a_end != *suff_end)
            return 0;
    
        suff_end--;
        a_end--;
    }
    
    return 1;
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
