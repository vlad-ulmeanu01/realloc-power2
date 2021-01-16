#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define name(x) (#x)

void check_ptr(void *p, char *s) {
  if (!p) {
    fprintf(stderr, "bad alloc: %s\n", s);
    exit(-1);
  }
}

typedef union smartunion {
  char C;
  short S;
  int I;
  long long L;
} smartunion;

typedef struct smart {
  void *ptr;
  int sz, sm; ///sm = size max
  size_t chk; ///marime o bucata: sizeof(char/short...)
} smart;

int smart_size(smart *s) {
  return s->sz;
}

void smart_push(smart *s, ...) {
  if (s->chk != 1 && s->chk != 2 && s->chk != 4 && s->chk != 8)
    s->chk = 1;

  if (s->sz >= s->sm) {
    if (s->sm > 0) {
      s->sm <<= 1;
      s->ptr = (void *)realloc(s->ptr, s->sm * s->chk);
      check_ptr(s->ptr, name(s->ptr));

      memset((char *)s->ptr + (s->sm >> 1) * s->chk, 0, (s->sm >> 1) * s->chk);
    } else {
      s->sm = 1;
      s->ptr = (void *)calloc(1, s->chk);
      check_ptr(s->ptr, name(s->ptr));
    }
  }

  va_list ap;
  va_start(ap, s);

  switch(s->chk) {
  case 1: ((char *)s->ptr)[s->sz] = (char) va_arg(ap, int); break;
  case 2: ((short *)s->ptr)[s->sz] = (short) va_arg(ap, int); break;
  case 4: ((int *)s->ptr)[s->sz] = (int) va_arg(ap, int); break;
  case 8: ((long long *)s->ptr)[s->sz] = (long long) va_arg(ap, long long); break;
  default: break;
  }

  s->sz++;
  va_end(ap);
}

void smart_set (smart *s, int ind, ...) {
  if (s->sz == 0)
    return;

  if (ind < 0) ind += ((-ind) / s->sz + 1) * s->sz;
  if (ind >= s->sz) ind %= s->sz;

  va_list ap;
  va_start(ap, ind);

  switch(s->chk) {
  case 1: ((char *)s->ptr)[ind] = (char) va_arg(ap, int); break;
  case 2: ((short *)s->ptr)[ind] = (short) va_arg(ap, int); break;
  case 4: ((int *)s->ptr)[ind] = (int) va_arg(ap, int); break;
  case 8: ((long long *)s->ptr)[ind] = (long long) va_arg(ap, long long); break;
  default: break;
  }

  va_end(ap);
}

smartunion smart_get(smart *s, int ind) {
  smartunion ans;
  if (s->sz == 0) {
    ans.C = 0;
    return ans;
  }

  if (ind < 0) ind += ((-ind) / s->sz + 1) * s->sz;
  if (ind >= s->sz) ind %= s->sz;

  switch(s->chk) {
  case 1: ans.C = ((char *)s->ptr)[ind]; break;
  case 2: ans.S = ((short *)s->ptr)[ind]; break;
  case 4: ans.I = ((int *)s->ptr)[ind]; break;
  case 8: ans.L = ((long long *)s->ptr)[ind]; break;
  default: ans.C = 0; break;
  }
  return ans;
}

void smart_pop (smart *s) {
  if (s->sz <= 0)
    return;
  switch(s->chk) {
  case 1: ((char *)s->ptr)[s->sz] = 0; break;
  case 2: ((short *)s->ptr)[s->sz] = 0; break;
  case 4: ((int *)s->ptr)[s->sz] = 0; break;
  case 8: ((long long *)s->ptr)[s->sz] = 0; break;
  default: break;
  }
  s->sz--;
}

void smart_init(smart *s, size_t newchk) {
  s->sz = 0;
  s->sm = 0;
  s->ptr = NULL;
  s->chk = newchk;
}

void smart_clear(smart *s) {
  if (s->sm != 0 || s->sz != 0 || s->ptr) {
    if (s->ptr)
      free(s->ptr);
    s->sz = 0;
    s->sm = 0;
    s->ptr = NULL;
  }
}

int main () {
  smart s;
  smart_init(&s, sizeof(int));
  int n;
  scanf("%d", &n);
  int i, z;
  for (i = 0; i < n; i++) {
    scanf("%d", &z);
    smart_push(&s, z);
  }
  for (i = 0; i < smart_size(&s); i++)
    printf("%d: %d\n", i, smart_get(&s, i).I);

  smart t;
  smart_init(&t, sizeof(char));

  scanf("%d", &n);
  fgetc(stdin);
  char ch;
  for (i = 0; i < n; i++) {
    ch = fgetc(stdin);
    smart_push(&t, ch);
  }
  smart_push(&t, '\0');
  puts((char *)t.ptr);

  smart sl;
  smart_init(&sl, sizeof(long long));
  scanf("%d", &n);
  smart_push(&sl, 0LL);

  long long x;
  for (i = 1; i <= n; i++) {
    scanf("%lld", &x);
    ((long long *)sl.ptr)[0] += x;
    smart_push(&sl, x);
  }

  printf("%lld\n", smart_get(&sl, -1).L);
  smart_pop(&sl);
  smart_pop(&sl);

  printf("%lld\n", smart_get(&sl, -2).L);
  smart_push(&sl, 999LL);

  smart_set(&sl, -1, 998LL);

  for (i = 0; i < smart_size(&sl); i++)
    printf("%lld\n", smart_get(&sl, i).L);

  smart_clear(&s);
  smart_clear(&t);
  smart_clear(&sl);
  return 0;
}
