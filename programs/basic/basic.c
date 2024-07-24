#include <stdio.h>
#include <string.h>

#define NUM_VARS 64  // 64
#define PRGM_SIZ 256 // 2000

#define slen(s) strlen(s)
#define scmp(s1, s2) (strcmp(s1, s2) == 0)
#define scpy(d, s) strcpy(d, s)

int isspc(char c) { return c == ' ' || c == '\n' || c == '\t'; }
int isdg(char c) { return c >= '0' && c <= '9'; }
int atoi_int(const char* s) { int i=0; for(;*s && isdg(*s);s++) i=i*10+(*s-'0'); return i; }

char* _CURTOK = NULL;

char* strtok_int(char* s) {
	if  (s != NULL) _CURTOK = s; // s=NULL, continue from last position
	if  (!(*_CURTOK)) return NULL; // Next char is NULL, return NULL
	for (;*_CURTOK && isspc(*_CURTOK);++_CURTOK); // cut off preceding spaces
	s = _CURTOK; // s=start of string
	for (;*_CURTOK && !isspc(*_CURTOK);++_CURTOK); // Advance to next space while there is next char that is not a space
	if  (!(*_CURTOK)) return NULL;
	*(_CURTOK++) = 0;
	return s;
}

// variation of strtok that can detect strings
char* sstrtok(char* s) {
	int a=0;
	if  (s != NULL) _CURTOK = s;
	if  (!(*_CURTOK)) return NULL;
	for (;*_CURTOK && isspc(*_CURTOK);++_CURTOK); s = _CURTOK;
	for (;*_CURTOK && (a||!isspc(*_CURTOK));++_CURTOK)
		if (*_CURTOK=='"') { if(!a) a=1; else break; }
	*(_CURTOK++) = 0;
	return s;
}
//////////////////////////////////////////////////////////////////////
// variable stuff
int rand() {
    return 3;
}

void srand(unsigned int seed) { }

unsigned int time(unsigned int* time_t) {
    return 1;
}

char varnames[NUM_VARS][8];
int  varcontent[NUM_VARS];

void initvars() {
    srand(time(NULL));
    for(int i=0; i<NUM_VARS; ++i)
        varnames[i][0] = 0;
}

int getvar(const char* s) {
	if (scmp(s, "RANDOM")) return rand();
	for (int i=0; i<NUM_VARS; ++i)
		if (scmp(s, varnames[i])) return varcontent[i];
	return 0;
}
int setvar(const char* s, int v) {
	for (int i=0; i<NUM_VARS; ++i)
		if (scmp(s, varnames[i])) { varcontent[i] = v; return 1; }
	for (int i=0; i<NUM_VARS; ++i)
		if (!varnames[i][0]) {
			scpy(varnames[i], s); varcontent[i] = v;
			return 1;
		}
	return 0; // returns 0 if it can't find a free variable slot
}


// init program memory
char prgm[PRGM_SIZ][NUM_VARS];

void initprgm() {
    for(int i=0; i<PRGM_SIZ; ++i)
        prgm[i][0] = 0;
}

// GOSUB stack
int _linestack[16];
int _linestackpos = 0;
void lnpush(int v) { _linestack[_linestackpos++] = v; }
int  lnpop()    { return _linestack[--_linestackpos]; }
//// data
// error
void berror(int linenum, const char* e) {
	if (linenum == -1)
		printf("ERROR: %s\n", e);
	else
		printf("ERROR AT %d: %s\n", linenum, e);
	exit(1);
}
// commands
int emath(char*);
typedef enum { PRINT,INPUT,VAR,IF,GOTO,GOSUB,RET,REM } BasicCommands;
const char* bcmds[] = {"PRINT","INPUT","VAR","IF","GOTO","GOSUB","RET","END"};
int getbcmd(const char* s) {
	for (int i = 0; i < REM+1; ++i) {
		if (scmp(s, bcmds[i]))
            return i;
    }
	return -1;
}
typedef int (*BasicCmd)(int,char*);
int cprint(int ln, char* s) {
	char* token = sstrtok(s);
	for (;token && *token;token = sstrtok(NULL))
	{
		if (token[0] == '"') {
			for(++token;*token&&*token!='"';++token) putc(*token);
		}
		else printf("%d", emath(token));
	}
	putc('\n');
	return ln;
}
int cinput(int ln, char* s) {
	char vs[16], vn[16]; scpy(vn, strtok_int(s));
	if (!vn) berror(ln, "INVALID ARGS");
	printf("%s? ", vn); fgets(vs, 15, stdin);
	setvar(vn, atoi_int(vs));
	return ln;
}
int cvar(int ln, char *s) {
	char *tok = strtok_int(s);
	if (!tok) berror(ln, "INVALID ARGS (1)");
	char vn[16]; scpy(vn, tok);
	tok = strtok_int(NULL);
	if (!tok) berror(ln, "INVALID ARGS (2)");
	setvar(vn, emath(tok));
	return ln;
}
int runcmd(int,char*);
int cif(int ln, char* s) {
	char *tok = strtok_int(s);
	if (!tok || !*_CURTOK) berror(ln, "INVALID IF STATEMENT");
	return emath(tok) ? runcmd(ln, _CURTOK) : ln;
}
int cgoto(int ln, char* s) {
	char *tok = strtok_int(s); if (!tok) berror(ln, "INVALID GOTO");
	return emath(tok)-1;
}
int cgosub(int ln, char* s) {
	char *tok = strtok_int(s); if (!tok) berror(ln, "INVALID GOSUB");
	int c=emath(tok); lnpush(ln); return c-1;
}
int cret(int ln, char* s) { return lnpop(); }
int crem(int ln, char* s) { exit(0); }

BasicCmd bfuncs[] = {cprint,cinput,cvar,cif,cgoto,cgosub,cret,crem};

int runcmd(int ln, char* s) {
    if(*s == 0)
        return ln;

	char buf[NUM_VARS];
    scpy(buf, s);
	char* token = sstrtok(buf);
    printf("Token: %x\n", token[0]);
	if (!token)
        return ln;
	int cmd;
    if ((cmd=getbcmd(token)) == -1) {
        berror(ln, "INVALID COMMAND");
    }
	return bfuncs[cmd](ln, _CURTOK);
}
// math operators
int iand(int a, int b) { return a & b; }
int  ior(int a, int b) { return a | b; }
int  igt(int a, int b) { return a > b; }
int  ilt(int a, int b) { return a < b; }
int ineq(int a, int b) { return a != b; }
int  ieq(int a, int b) { return a == b; }
int imod(int a, int b) { return a % b; }
int imul(int a, int b) { return a * b; }
int idiv(int a, int b) { return a / b; }
int iadd(int a, int b) { return a + b; }
int isub(int a, int b) { return a - b; }
const char* mathops = "&|><~=%*/+-";
typedef int (*Mathfunc)(int,int);
Mathfunc mathfuncs[] = {iand,ior,igt,ilt,ineq,ieq,imod,imul,idiv,iadd,isub};
int emath(char* s) {
	if (!*s) return 0;
	for (int c,i=0; (c=mathops[i]); ++i)
		for (int j=0; s[j]; ++j)
			if (s[j] == c) {
				s[j] = 0;
				return mathfuncs[i](emath(s), emath(s+j+1));
			}
	return isdg(*s) ? atoi_int(s) : getvar(s);
}

//////////////////////////////////////////////////////////////////////
///
void run_basic()
{
	for (int i = 0; i < PRGM_SIZ; ++i)
		i = runcmd(i, prgm[i]);
}

void read_program(int stream)
{
	char buffer[NUM_VARS], *bptr, *token;
	int ln=0, pln=0;

	while (fgets(bptr=buffer, NUM_VARS-1, stream))
	{
		++ln;
        for (;*bptr && isspc(*(bptr)); ++bptr);
		if (!*bptr || *bptr == '#')
            continue;

		if (!isdg(*bptr)) berror(ln, "PARSER: MISSING NUMBER");

		token = strtok_int(bptr);
		pln = atoi_int(token);
		scpy(prgm[pln], _CURTOK);
        printf("Ln: %d\n", pln);
	}
}

// Used for testing
int emath_test()
{
	printf("Math evaluation mode.\n");

	char buffer[NUM_VARS];
	while (read(stdin, buffer, NUM_VARS-1)) printf(" = %d\n", emath(buffer));
	return 0;
}

int main(int argc, char** argv)
{
	initprgm(); initvars();
	if (argc == 2 && scmp(argv[1], "-emath")) { return emath_test(); }
	int f;

    if(argc == 2) {
        f = open(argv[1], O_RDONLY);
        if (!f || f == -1)
            berror(-1, "FILE UNREADABLE");
    } else {
        f = stdin;
    }

	read_program(f); run_basic();
	return 0;
}
