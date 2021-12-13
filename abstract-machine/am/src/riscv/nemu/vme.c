#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)
#define exp2(x) (1 << (x))

typedef struct 
{
  union 
  {
    struct{
      uint32_t page_offset : 12;
      uint32_t vpn0        : 10;
      uint32_t vpn1        : 10;
    } va;
    uint32_t val;
  } vaddr_;
} vaddr;

typedef struct 
{
  union 
  {
    struct{
      uint32_t page_offset : 12;
      uint32_t ppn0        : 10;
      uint32_t ppn1        : 10;
    } pa;
    uint32_t val;
  } paddr_;
} paddr;

typedef struct 
{
  union 
  {
    uint32_t V        : 1;
    uint32_t not_used : 9;
    uint32_t ppn0     : 10;
    uint32_t ppn1     : 12;
  } pte_;
  uint32_t val;
} pte;



vaddr va_tmp = {};
paddr pa_tmp = {}, pte_addr_tmp = {};
pte pte1 = {}, pte2 = {};

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  printf("__riscv_xlen = %d\n", __riscv_xlen);
  printf("set satp as %x\n", (mode | ((uintptr_t)pdir >> 12)));
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE); // need to + 1?

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  // to fill the page table
  va_tmp.vaddr_.val = (uint32_t)va;
  pa_tmp.paddr_.val = (uint32_t)pa;
  assert(va_tmp.vaddr_.va.page_offset == pa_tmp.paddr_.pa.page_offset);

  // not sure about the satp_addr
  uint32_t satp_addr = (uint32_t)as->ptr;
  uint32_t pte_ppn_addr = satp_addr + (1 + va_tmp.vaddr_.va.vpn1) * PGSIZE; // try to give the addr of second-level page table
  uint32_t *pte1_addr = (uint32_t *)(satp_addr + va_tmp.vaddr_.va.vpn1 * PTESIZE); // PTESIZE == 4 !!!
  pte1.val = *pte1_addr;
  if (pte1.pte_.V == 0){
    pte1.pte_.V = 1;
    pte_addr_tmp.paddr_.val = pte_ppn_addr; 
    pte1.pte_.ppn0 = pte_addr_tmp.paddr_.pa.ppn0;
    pte1.pte_.ppn1 = pte_addr_tmp.paddr_.pa.ppn1;
    assert(pte_addr_tmp.paddr_.pa.page_offset == 0);
    *pte1_addr = pte1.val;
    printf("load level 1 page table entry at %p value %x\n", pte1_addr, *pte1_addr);
  }
  assert(*pte1_addr == pte1.val);
  uint32_t *pte2_addr = (uint32_t *)((pte1.pte_.ppn1 * exp2(10) + pte1.pte_.ppn0) * PGSIZE + va_tmp.vaddr_.va.vpn0 * PTESIZE);
  pte2.val = *pte2_addr;
  pte2.pte_.ppn0 = pa_tmp.paddr_.pa.ppn0;
  pte2.pte_.ppn1 = pa_tmp.paddr_.pa.ppn1;
  pte2.pte_.V    = 1; 
  *pte2_addr = pte2.val;
  printf("load level 2 page table entry at %p value %x\n", pte2_addr, *pte2_addr);
}

#define CONTEXT_SIZE  (32 + 3 + 1)
#define OFFSET_SP     2 
#define OFFSET_A0     10
#define OFFSET_CAUSE  32
#define OFFSET_STATUS 33
#define OFFSET_EPC    34

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  uint32_t *stack_p = kstack.end;
  // uint32_t *heap_p  = kstack.start;
  stack_p -= CONTEXT_SIZE;
  *(stack_p + OFFSET_EPC) = (uintptr_t)entry - 4;
  *(stack_p + OFFSET_SP) = (uintptr_t)stack_p;
  
  // Context cp is set in nanos
  // *heap_p = (uintptr_t)stack_p;
  return (Context *)stack_p;
  return NULL;
}
