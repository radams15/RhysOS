/* tinylisp-commented.c with NaN boxing by Robert A. van Engelen 2022 */
/* tinylisp.c but adorned with comments in an (overly) verbose C style */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int running = 1;

#ifdef RHYSOS

#define getchar() getch()
#define putchar(c) putc(c)
#define strtol(a, b, c) strtoi(a, b, c)

void exit(char code) {
    running = 0;
}

#endif

/* we only need two types to implement a Lisp interpreter:
        I    unsigned integer (either 16 bit, 32 bit or 64 bit unsigned)
        Expr    Lisp expression (double with NaN boxing)
   I variables and function parameters are named as follows:
        i    any unsigned integer, e.g. a NaN-boxed ordinal value
        t    a NaN-boxing tag
   Expr variables and function parameters are named as follows:
        x,y  any Lisp expression
        n    number
        t    list
        f    function or Lisp primitive
        p    pair, a cons of two Lisp expressions
        e,d  environment, a list of pairs, e.g. created with (define v x)
        v    the name of a variable (an atom) or a list of variables */
#define I unsigned int

struct Val {
    I ord;
    I tag;
};

#define Expr struct Val

/* T(x) returns the tag bits of a NaN-boxed Lisp expression x */
#define T(x) x.tag

/* number of cells for the shared stack and atom heap, increase N as desired */
#define N 1024

/* hp: heap pointer, A+hp with hp=0 points to the first atom string in cell[]
   sp: stack pointer, the stack starts at the top of cell[] with sp=N
   safety invariant: hp <= sp<<3 */
I hp = 0, sp = N;

enum {
    ATOM = 0x7ff8,
    PRIM = 0x7ff9,
    CONS = 0x7ffa,
    CLOS = 0x7ffb,
    NIL = 0x7ffc
};

/* cell[N] array of Lisp expressions, shared by the stack and atom heap */
Expr cell[N];

/* Lisp constant expressions () (nil), #t, ERR, and the global environment env */
Expr nil, tru, err, env;

/* NaN-boxing specific functions:
   box(t,i): returns a new NaN-boxed double with tag t and ordinal i
   ord(x):   returns the ordinal of the NaN-boxed double x
   num(n):   convert or check number n (does nothing, e.g. could check for NaN)
   equ(x,y): returns nonzero if x equals y */
Expr box(I t, I i) {
  return (Expr){
      i,
      t
  };
}

I ord(Expr x) {
  return x.ord;      /* the return value is narrowed to 32-bit unsigned integer to remove the tag */
}

Expr num(Expr n) {
  return n;
}

I equ(Expr x, Expr y) {
  return ord(x) == ord(y);
}

I dups(const char* in) {
    int len = strlen(in) + 1;

    char* out = malloc(len * sizeof(char*));

    strncpy(out, in, len);

    return (I) out;
}

/* interning of atom names (Lisp symbols), returns a unique NaN-boxed ATOM */
Expr atom(const char *s) {
  I i = 0;
  while (i < hp && ord(cell[i]) != 0 && strcmp((char*) ord(cell[i]), s) != 0)              /* search for a matching atom name on the heap */
    i ++;

  if (i == hp) {                                /* if not found */
      hp++;             /*   allocate and add a new atom name to the heap */
      cell[i] = box(ATOM, dups(s));
      if (hp > sp<<3)                             /* abort when out of memory */
          abort();
  }

  return box(ATOM, i);
}

/* construct pair (x . y) returns a NaN-boxed CONS */
Expr cons(Expr x, Expr y) {
  cell[--sp] = x;                               /* push the car value x */
  cell[--sp] = y;                               /* push the cdr value y */
  if (hp > sp<<3)                               /* abort when out of memory */
    abort();
  return box(CONS, sp);
}

/* return the car of a pair or ERR if not a pair */
Expr car(Expr p) {
    if((T(p) & ~(CONS^CLOS)) == CONS)
        return cell[ord(p)+1];
    return err;
}

/* return the cdr of a pair or ERR if not a pair */
Expr cdr(Expr p) {
  return (T(p) & ~(CONS^CLOS)) == CONS ? cell[ord(p)] : err;
}

/* construct a pair to add to environment e, returns the list ((v . x) . e) */
Expr pair(Expr v, Expr x, Expr e) {
  return cons(cons(v, x), e);
}

/* construct a closure, returns a NaN-boxed CLOS */
Expr closure(Expr v, Expr x, Expr e) {
  return box(CLOS, ord(pair(v, x, equ(e, env) ? nil : e)));
}

/* look up a symbol in an environment, return its value or ERR if not found */
Expr assoc(Expr v, Expr e) {
  while (T(e) == CONS && !equ(v, car(car(e))))
    e = cdr(e);
  return T(e) == CONS ? cdr(car(e)) : err;
}

/* not(x) is nonzero if x is the Lisp () empty list */
I not(Expr x) {
  return T(x) == NIL;
}

/* let(x) is nonzero if x is a Lisp let/let* pair */
I let(Expr x) {
  return T(x) != NIL && !not(cdr(x));
}

/* return a new list of evaluated Lisp expressions t in environment e */
Expr eval(Expr, Expr);
Expr evlis(Expr t, Expr e) {
  if(T(t) == CONS)
      return cons(eval(car(t), e), evlis(cdr(t), e));
  return T(t) == ATOM ? assoc(t,e) : nil;
}

/* Lisp primitives:
   (eval x)            return evaluated x (such as when x was quoted)
   (quote x)           special form, returns x unevaluated "as is"
   (cons x y)          construct pair (x . y)
   (car p)             car of pair p
   (cdr p)             cdr of pair p
   (add n1 n2 ... nk)  sum of n1 to nk
   (sub n1 n2 ... nk)  n1 minus sum of n2 to nk
   (mul n1 n2 ... nk)  product of n1 to nk
   (div n1 n2 ... nk)  n1 divided by the product of n2 to nk
   (int n)             integer part of n
   (< n1 n2)           #t if n1<n2, otherwise ()
   (eq? x y)           #t if x equals y, otherwise ()
   (not x)             #t if x is (), otherwise ()
   (or x1 x2 ... xk)   first x that is not (), otherwise ()
   (and x1 x2 ... xk)  last x if all x are not (), otherwise ()
   (cond (x1 y1)
         (x2 y2)
         ...
         (xk yk))      the first yi for which xi evaluates to non-()
   (if x y z)          if x is non-() then y else z
   (let* (v1 x1)
         (v2 x2)
         ...
         y)            sequentially binds each variable v1 to xi to evaluate y
   (lambda v x)        construct a closure
   (define v x)        define a named value globally */
Expr f_eval(Expr t, Expr e) {
  return eval(car(evlis(t, e)), e);
}

Expr f_quote(Expr t, Expr _) {
  return car(t);
}

Expr f_cons(Expr t, Expr e) {
  t = evlis(t, e);
  return cons(car(t), car(cdr(t)));
}

Expr f_car(Expr t, Expr e) {
  return car(car(evlis(t, e)));
}

Expr f_cdr(Expr t, Expr e) {
  return cdr(car(evlis(t, e)));
}

Expr f_add(Expr t, Expr e) {
  Expr n;
  t = evlis(t, e);
  n = car(t);
  while (!not(t = cdr(t)))
    n.ord += car(t).ord;
  return num(n);
}

Expr f_sub(Expr t, Expr e) {
  Expr n;
  t = evlis(t, e);
  n = car(t);
  while (!not(t = cdr(t)))
    n.ord -= car(t).ord;
  return num(n);
}

Expr f_mul(Expr t, Expr e) {
  Expr n;
  t = evlis(t, e);
  n = car(t);
  while (!not(t = cdr(t)))
    n.ord *= car(t).ord;
  return num(n);
}

Expr f_div(Expr t, Expr e) {
  Expr n;
  t = evlis(t, e);
  n = car(t);
  while (!not(t = cdr(t)))
    n.ord /= car(t).ord;
  return num(n);
}

Expr f_pow(Expr t, Expr e) {
    Expr n;
    t = evlis(t, e);
    n = car(t);
    while (!not(t = cdr(t))) {
        n.ord = (I) pow((I) n.ord, (I) car(t).ord);
    }
    return num(n);
}

Expr f_int(Expr t, Expr e) {
    Expr lis = evlis(t, e);
    Expr n = car(lis);
    return n;
}

Expr f_lt(Expr t, Expr e) {
  return t = evlis(t, e), car(t).ord - car(cdr(t)).ord < 0 ? tru : nil;
}

Expr f_eq(Expr t, Expr e) {
  return t = evlis(t, e), equ(car(t), car(cdr(t))) ? tru : nil;
}

Expr f_not(Expr t, Expr e) {
  return not(car(evlis(t, e))) ? tru : nil;
}

Expr f_or(Expr t, Expr e) {
  Expr x = nil;
  while (T(t) != NIL && not(x = eval(car(t),e)))
    t = cdr(t);
  return x;
}

Expr f_and(Expr t, Expr e) {
  Expr x = nil;
  while (T(t) != NIL && !not(x = eval(car(t),e)))
    t = cdr(t);
  return x;
}

Expr f_cond(Expr t, Expr e) {
  while (T(t) != NIL && not(eval(car(car(t)), e)))
    t = cdr(t);
  return eval(car(cdr(car(t))), e);
}

Expr f_if(Expr t, Expr e) {
  return eval(car(cdr(not(eval(car(t), e)) ? cdr(t) : t)), e);
}

Expr f_leta(Expr t, Expr e) {
  for (; let(t); t = cdr(t))
    e = pair(car(car(t)), eval(car(cdr(car(t))), e), e);
  return eval(car(t), e);
}

Expr f_lambda(Expr t, Expr e) {
  return closure(car(t), car(cdr(t)), e);
}

Expr f_define(Expr t, Expr e) {
  env = pair(car(t), eval(car(cdr(t)), e), env);
  return car(t);
}

Expr f_exit(Expr t, Expr e) {
    Expr n = car(t);

    exit((int) n.ord);
}

/* table of Lisp primitives, each has a name s and function pointer f */
struct {
  const char *s;
  Expr (*f)(Expr, Expr);
} prim[] = {
  {"eval",   f_eval},
  {"quote",  f_quote},
  {"cons",   f_cons},
  {"car",    f_car},
  {"cdr",    f_cdr},
  {"+",      f_add},
  {"-",      f_sub},
  {"*",      f_mul},
  {"/",      f_div},
  {"^",      f_pow},
  {"int",    f_int},
  {"<",      f_lt},
  {"eq?",    f_eq},
  {"or",     f_or},
  {"and",    f_and},
  {"not",    f_not},
  {"cond",   f_cond},
  {"if",     f_if},
  {"let*",   f_leta},
  {"lambda", f_lambda},
  {"define", f_define},
  {"exit", f_exit},
  {0}};

/* create environment by extending e with variables v bound to values t */
Expr bind(Expr v, Expr t, Expr e) {
  return T(v) == NIL ? e :
         T(v) == CONS ? bind(cdr(v), cdr(t), pair(car(v), car(t), e)) :
         pair(v, t, e);
}

/* apply closure f to arguments t in environemt e */
Expr reduce(Expr f, Expr t, Expr e) {
  return eval(cdr(car(f)), bind(car(car(f)), evlis(t, e), not(cdr(f)) ? env : cdr(f)));
}

/* apply closure or primitive f to arguments t in environment e, or return ERR */
Expr apply(Expr f, Expr t, Expr e) {
  return T(f) == PRIM ? prim[ord(f)].f(t, e) :
         T(f) == CLOS ? reduce(f, t, e) :
         err;
}

/* evaluate x and return its value in environment e */
Expr eval(Expr x, Expr e) {
  if(T(x) == ATOM)
      return assoc(x, e);
  else {
      if(T(x) == CONS)
            return apply(eval(car(x), e), cdr(x), e);
      else
        return x;
  }
}

/* tokenization buffer and the next character that we are looking at */
char buf[40], see = ' ';

/* advance to the next character */
void look() {
  int c = getchar();
  see = c;
  if (c == EOF)
    exit(0);
}

/* return nonzero if we are looking at character c, ' ' means any white space */
I seeing(char c) {
  return c == ' ' ? see > 0 && see <= c : see == c;
}

/* return the look ahead character from standard input, advance to the next */
char get() {
  char c = see;
  look();
  return c;
}

/* tokenize into buf[], return first character of buf[] */
char scan() {
  I i = 0;
  while (seeing(' '))
    look();
  if (seeing('(') || seeing(')') || seeing('\''))
    buf[i++] = get();
  else
    do
      buf[i++] = get();
    while (i < 39 && !seeing('(') && !seeing(')') && !seeing(' '));
  buf[i] = 0;
  return *buf;
}

/* return the Lisp expression read from standard input */
Expr parse();
Expr Read() {
  scan();
  return parse();
}

/* return a parsed Lisp list */
Expr list() {
  Expr x;
  if (scan() == ')')
    return nil;
  if (!strcmp(buf, ".")) {
    x = Read();
    scan();
    return x;
  }
  x = parse();
  return cons(x, list());
}

/* return a parsed Lisp expression x quoted as (quote x) */
Expr quote() {
  return cons(atom("quote"), cons(Read(), nil));
}

/* return a parsed atomic Lisp expression (a number or an atom) */
Expr atomic() {
  I x;
  char* end;

  x = strtol(buf, &end, 10);
  if(end-buf != 0)
      return box(0, x);

  return atom(buf);
}

/* return a parsed Lisp expression */
Expr parse() {
  return *buf == '(' ? list() :
         *buf == '\'' ? quote() :
         atomic();
}

/* display a Lisp list t */
void print_expr(Expr);

void printlist(Expr t) {
  for (putchar('('); ; putchar(' ')) {
      print_expr(car(t));
    t = cdr(t);
    if (T(t) == NIL)
      break;
    if (T(t) != CONS) {
      printf(" . ");
        print_expr(t);
      break;
    }
  }
  putchar(')');
}

/* display a Lisp expression x */
void print_expr(Expr x) {
  if (T(x) == NIL)
    printf("()");
  else if (T(x) == ATOM)
    printf("%s", (char*) ord(cell[ord(x)]));
  else if (T(x) == PRIM)
    printf("<%s>", prim[ord(x)].s);
  else if (T(x) == CONS)
    printlist(x);
  else if (T(x) == CLOS)
    printf("{%u}", ord(x));
  else
    printf("%d", x.ord);
}

/* garbage collection removes temporary cells, keeps global environment */
void gc() {
  sp = ord(env);
}

/* Lisp initialization and REPL */
int main() {
  I i;
  printf("tinylisp");
  nil = box(NIL, 0);
  err = atom("ERR");
  tru = atom("#t");
  env = pair(tru, tru, nil);

  for (i = 0; prim[i].s; ++i)
    env = pair(atom(prim[i].s), box(PRIM, i), env);

  while (running) {
    printf("\n%u>", sp-hp/sizeof(Expr));
      print_expr(eval(Read(), env));
    gc();
  }
}
