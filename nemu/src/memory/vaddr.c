#include <isa.h>
#include <memory/paddr.h>

#define mode_mask 0x80000000

word_t vaddr_ifetch(vaddr_t addr, int len) {
  uint32_t satp = cpu.satp;
  if ((satp & mode_mask)){
    
  }
  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  // int mm_check = isa_mmu_check(vaddr_t, len, MEM_TYPE_READ);
    // printf("__riscv_xlen = %d\n", 32);
  uint32_t satp = cpu.satp;
  if ((satp & mode_mask)){
    
  }
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  uint32_t satp = cpu.satp;
  if ((satp & mode_mask)){
    
  }
  paddr_write(addr, len, data);
}
