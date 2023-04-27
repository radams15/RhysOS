void memset(int *dest, int val, int len) {
    int *temp = (int *)dest;
    for ( ; len != 0; len--) *temp++ = val;
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

void memcpy(int *dest, int *src, unsigned int n) {
  while (n--) {
    *dest++ = *src++;
  }
}

void clear(int *ptr, unsigned int n) {
  memset(ptr, 0, n);
}
