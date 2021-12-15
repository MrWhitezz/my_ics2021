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
int mm_brk(intptr_t brk) {
  printf("brk = %d\n", brk);
  assert(current->program_brk + brk < (uint32_t)current->as.area.end);
  if (brk > 0){
    int nr_page = ((current->program_brk + brk - 1) / PGSIZE) - ((current->max_brk - 1) / PGSIZE);
    if (nr_page > 0){
      void *p_page = pg_alloc(nr_page);
      void *v_page = (void *)(ROUNDUP(current->max_brk, PGSIZE));
      printf("v_page + (nrpage - 1) * PGSIZE = %x\n", (uintptr_t)(v_page + (nr_page - 1) * PGSIZE));
      printf("(current->program_brk + brk)) = %x\n", (current->program_brk + brk));
      assert((uintptr_t)(v_page + (nr_page - 1) * PGSIZE) / PGSIZE == (current->program_brk + brk - 1) / PGSIZE);
      for (int i = 0; i < nr_page; ++i){
        map(&current->as, v_page + i * PGSIZE, p_page + i * PGSIZE, MMAP_READ | MMAP_WRITE);
      }
    }
  }
  current->program_brk += brk;
  current->max_brk = MAX(current->max_brk, current->program_brk);
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
