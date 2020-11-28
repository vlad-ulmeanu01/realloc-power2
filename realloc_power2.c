#include <stdio.h>
#include <stdlib.h>
#define name(x) (#x)

void check_ptr (void* ptr, char* s) {
  if (ptr == NULL) {
    fprintf(stderr, "%s nedeclarat corect.\n", s);
    exit(0);
  }
}

/**
pointer la functie de genul: tip_return (*nume_functie) (tip_param1, tip_param2, ...)
apelat cu (*nume_functie)(param1, param2, ...)
transmitere adresa functie prin &nume_functie
**/

typedef struct smart {
  int* ptr;
  int sz, sm; ///sm = size max
  int (*size)(struct smart*);
  void (*push)(struct smart*, int);
} smart;

int smart_size (smart* s) {
  return s->sz;
}

void smart_push (smart* s, int val) {
  if (s->sz >= s->sm) {
    if (s->sm > 0) {
      s->sm *= 2;
      s->ptr = realloc(s->ptr, s->sm * sizeof(int));
      check_ptr(s->ptr, name(s->ptr));
    } else {
      s->sm = 1;
      s->ptr = malloc(sizeof(int));
      check_ptr(s->ptr, name(s->ptr));
    }
  }
  s->ptr[s->sz++] = val;
}

void smart_init (smart* s) {
  s->sz = 0;
  s->sm = 0;
  s->size = smart_size;
  s->push = smart_push;
}

int main() {
  smart v; smart_init(&v);
  int n; scanf("%d", &n);
  int i, j, z;
  for (i = 0; i < n; i++) {
    scanf("%d", &z);
    v.push(&v, z);
    printf("i %d size %d sm %d\n", i, v.size(&v), v.sm);
  }
  for (i = 0; i < n; i++) printf("%d ", v.ptr[i]);
  puts("");
  free(v.ptr);
  return 0;
}
/**
14
493 2 -3 29083 127 34 203 3710 4387 2031 73481 20 2 -5
**/
