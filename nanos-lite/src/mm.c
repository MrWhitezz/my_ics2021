#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  assert(pf + nr_page * PGSIZE < heap.end);
  pf += nr_page * PGSIZE;
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
  // not sure about free pages start.
  pf = (void *)ROUNDUP(heap.start + (heap.end - heap.start) / 3, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
