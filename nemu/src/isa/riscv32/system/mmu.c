#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

extern char _pmem_start;
#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_END  ((uintptr_t)&_pmem_start + PMEM_SIZE) // 0x80000000 to 0x88000000


paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  return MEM_RET_FAIL;
}

int isa_mmu_check(vaddr_t vaddr, int len, int type){
  return MMU_DIRECT;
}
