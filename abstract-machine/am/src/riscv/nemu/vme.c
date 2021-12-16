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
      struct
      {
      uint32_t V        : 1;
      uint32_t not_used : 9;
      uint32_t ppn0     : 10;
      uint32_t ppn1     : 12;
        /* data */
      } pte;
    uint32_t val;
  } pte_;
} pte;



vaddr va_tmp = {.vaddr_.val = 0};
paddr pa_tmp = {.paddr_.val = 0}, pte_addr_tmp = {.paddr_.val = 0};
pte pte1 = {.pte_.val = 0}, pte2 = {.pte_.val = 0};

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  // printf("__riscv_xlen = %d\n", __riscv_xlen);
  // printf("set satp as %x\n", (mode | ((uintptr_t)pdir >> 12)));
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
  // printf("set satp as %x\n", (mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  // kas.ptr = pgalloc_f(PGSIZE); // don't know why so large the space 
  kas.ptr = pgalloc_f(1); 

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
  // PTE *updir = (PTE*)(pgalloc_usr(PGSIZE)); // don't know why so large
  PTE *updir = (PTE*)(pgalloc_usr(1));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space, This is level1 page table!!!!
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  // printf("begin __am_switch\n");
  // printf("vme_enable = %d\n", vme_enable);
  // printf("begin __am_switch\n");
  // printf("c->pdir %p\n", c->pdir);
  // printf("begin __am_switch\n");
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

  // assert(va_tmp.vaddr_.va.vpn1 != 0x3ff);
  // uint32_t pte_ppn_addr = satp_addr + (1 + va_tmp.vaddr_.va.vpn1) * PGSIZE; // try to give the addr of second-level page table

  // printf("satp_addr = %x\n", satp_addr);
  // printf("pte_ppn_addr allocated by OS at %x\n", pte_ppn_addr);
  uint32_t *pte1_addr = (uint32_t *)(satp_addr + va_tmp.vaddr_.va.vpn1 * PTESIZE); // PTESIZE == 4 !!!
  pte1.pte_.val = *pte1_addr; // not secure
  if (pte1.pte_.pte.V == 0){
    pte1.pte_.pte.V = 1;
    void *p_page = pgalloc_usr(1);
    // pte_addr_tmp.paddr_.val = pte_ppn_addr; 
    pte_addr_tmp.paddr_.val = (uint32_t)p_page; 
    pte1.pte_.pte.ppn0 = pte_addr_tmp.paddr_.pa.ppn0;
    pte1.pte_.pte.ppn1 = pte_addr_tmp.paddr_.pa.ppn1;
    assert(pte_addr_tmp.paddr_.pa.page_offset == 0);
    *pte1_addr = pte1.pte_.val;
    printf("va: %x pa: %x\n", va, pa);
    printf("load level 1 page table entry at %p value %x >> 2\n", pte1_addr, *pte1_addr << 2);
  }
  // printf("level 1 page table entry at %p value %x >> 2\n", pte1_addr, *pte1_addr << 2);
  assert(*pte1_addr == pte1.pte_.val);
  uint32_t *pte2_addr = (uint32_t *)((pte1.pte_.pte.ppn1 * exp2(10) + pte1.pte_.pte.ppn0) * PGSIZE + va_tmp.vaddr_.va.vpn0 * PTESIZE);
  pte2.pte_.val = *pte2_addr;
  pte2.pte_.pte.ppn0 = pa_tmp.paddr_.pa.ppn0;
  pte2.pte_.pte.ppn1 = pa_tmp.paddr_.pa.ppn1;
  pte2.pte_.pte.V    = 1; 
  *pte2_addr = pte2.pte_.val;
  // printf("load level 2 page table entry at %p value %x >> 2\n", pte2_addr, *pte2_addr << 2);
}

#define CONTEXT_SIZE  (32 + 3 + 1 + 1)
#define OFFSET_SP     2 
#define OFFSET_A0     10
#define OFFSET_CAUSE  32
#define OFFSET_STATUS 33
#define OFFSET_EPC    34
#define OFFSET_PDIR   35

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  assert(as != NULL);
  uint32_t *stack_p = kstack.end;
  // uint32_t *heap_p  = kstack.start;
  stack_p -= CONTEXT_SIZE;
  *(stack_p + OFFSET_EPC)    = (uintptr_t)entry - 4;
  *(stack_p + OFFSET_SP)     = (uintptr_t)stack_p;
  *(stack_p + OFFSET_STATUS) = (uintptr_t)0x1808;
  *(stack_p + OFFSET_PDIR)   = (uintptr_t)as->ptr;
  
  // Context cp is set in nanos
  // *heap_p = (uintptr_t)stack_p;
  return (Context *)stack_p;
  return NULL;
}
