#include <memory.h>
#include <proc.h>

extern char _end;
static void *pf = NULL;

void* new_page(size_t nr_page) {
  assert(pf + nr_page * PGSIZE < heap.end);
  pf += nr_page * PGSIZE;
  return pf;
}

#ifdef HAS_VME
void* pg_alloc(int n) {
  void *old_pf = pf;
  new_page(n);
  memset(old_pf, 0, n * PGSIZE);
  return old_pf;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  printf("brk = %x\n", brk);
  int nr_page = ((current->max_brk + brk) / PGSIZE) - (current->max_brk / PGSIZE);
  void *p_page = pg_alloc(nr_page);
  void *v_page = (void *)(ROUNDUP(current->max_brk, PGSIZE));
  // printf("v_page + nrpage * PGSIZE = %x\n", (uintptr_t)(v_page + nr_page * PGSIZE));
  printf("(current->max_brk + brk)) = %x\n", (current->max_brk + brk));
  assert((uintptr_t)(v_page + (nr_page - 1) * PGSIZE) / PGSIZE == (current->max_brk + brk) / PGSIZE);
  for (int i = 0; i < nr_page; ++i){
    map(&current->as, v_page + i * PGSIZE, p_page + i * PGSIZE, MMAP_READ | MMAP_WRITE);
  }
  current->max_brk += brk;
  assert(current->max_brk < (uint32_t)current->as.area.end);
  return 0;
}

void init_mm() {
  // not sure about free pages start.
  // pf = (void *)ROUNDUP(heap.start + (heap.end - heap.start) / 3, PGSIZE);
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
