#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  if (pf == NULL)
    pf = (void *)(uintptr_t)heap.end;
  // wierd bug
  printf("heap.end = %p\n", heap.end);
  assert(pf - nr_page * PGSIZE < pf);
  printf("pf = %p\n", pf);
  pf -= nr_page * PGSIZE;
  printf("pf = %p\n", pf);
  return pf;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  return NULL;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
