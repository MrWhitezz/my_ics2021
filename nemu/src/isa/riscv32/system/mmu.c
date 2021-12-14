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

#define SATP_PPN_MASK 0xfffff
#define mode_mask     0x80000000
#define PTESIZE       4
#define exp2(x)       (1 << (x))

vaddr va_tmp;
paddr pa_tmp;
pte pte1, pte2;

bool is_in_pmem(vaddr_t va_beg, vaddr_t va_end){
  return (va_beg >= PMEM_BEG && va_end <= PMEM_END);
}

bool is_in_mmio(vaddr_t va_beg, vaddr_t va_end){
  return (va_beg >= MMIO_BASE && va_end <= MMIO_END);
}

bool is_in_FB(vaddr_t va_beg, vaddr_t va_end){
  return (va_beg >= FB_ADDR && va_end <= FB_END);
}

long long cnt_trans = 0;

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  uint32_t satp_ppn = cpu.satp & SATP_PPN_MASK;
  assert(cpu.satp & mode_mask);   assert(PAGE_SIZE == 4096);
  va_tmp.vaddr_.val = vaddr;

  paddr_t pte1_addr = (satp_ppn * PAGE_SIZE) + (va_tmp.vaddr_.va.vpn1 * PTESIZE);
  pte1.pte_.val = paddr_read(pte1_addr, PTESIZE);
  assert(pte1.pte_.pte.V == 1);
  paddr_t pte2_addr = (pte1.pte_.pte.ppn1 * exp2(10) + pte1.pte_.pte.ppn0) * PAGE_SIZE + va_tmp.vaddr_.va.vpn0 * PTESIZE;
  pte2.pte_.val = paddr_read(pte2_addr, PTESIZE);
  assert(pte2.pte_.pte.V == 1);

  paddr_t pa = ((pte2.pte_.pte.ppn1 * exp2(10) + pte2.pte_.pte.ppn0) * PAGE_SIZE) + va_tmp.vaddr_.va.page_offset;

  if (cnt_trans++ % 100000 == 0 && type == MEM_TYPE_WRITE && !is_in_pmem(vaddr, vaddr))
    printf("Translate %lld times success at pa %x\n", cnt_trans, pa);
  assert(vaddr == pa);
  return pa;

  assert(0);
  return MEM_RET_FAIL;
}



int isa_mmu_check(vaddr_t vaddr, int len, int type){
  vaddr_t va_end = vaddr + len;
  if (is_in_pmem(vaddr, va_end) || is_in_mmio(vaddr, va_end) || is_in_FB(vaddr, va_end)){
    return MMU_DIRECT;
  }
  else {
    return MMU_TRANSLATE;
  }
  return MMU_DIRECT;
}
