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
    printf("%d: %d\n", i, ((int *)s.ptr)[i]);

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

  for (i = 0; i < smart_size(&sl); i++)
    printf("%lld\n", ((long long *)sl.ptr)[i]);

  smart_clear(&s);
  smart_clear(&t);
  smart_clear(&sl);
  return 0;
}
