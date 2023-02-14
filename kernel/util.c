void memset(int *dest, int val, int len) {
    int *temp = (int *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}
