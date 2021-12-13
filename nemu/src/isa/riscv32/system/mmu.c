#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_BEG  CONFIG_MBASE
#define PMEM_END  (PMEM_BEG + PMEM_SIZE) // 0x80000000 to 0x88000000
#define MMIO_BASE 0xa0000000
#define MMIO_END  (MMIO_BASE + 0x1000)
#define FB_ADDR   (MMIO_BASE + 0x1000000)
#define FB_END    (FB_ADDR   + 0x200000)


paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  assert(0);
  return MEM_RET_FAIL;
}

bool is_in_pmem(vaddr_t va_beg, vaddr_t va_end){
  return (va_beg >= PMEM_BEG && va_end <= PMEM_END);
}

bool is_in_mmio(vaddr_t va_beg, vaddr_t va_end){
  return (va_beg >= MMIO_BASE && va_end <= MMIO_END);
}

bool is_in_FB(vaddr_t va_beg, vaddr_t va_end){
  return (va_beg >= FB_ADDR && va_end <= FB_END);
}

int isa_mmu_check(vaddr_t vaddr, int len, int type){
  vaddr_t va_end = vaddr + len;
  if (is_in_pmem(vaddr, va_end) || is_in_mmio(vaddr, va_end) || is_in_FB(vaddr, va_end)){
    return MMU_DIRECT;
  }
  else {
    printf("translate\n");
    return MMU_TRANSLATE;
  }
  return MMU_DIRECT;
}
